# -*- coding: utf-8 -*-

import os
import sys
import time
import threading
import datetime
import time

from pyqtgraph.Qt import QtGui, QtCore
import serial.tools.list_ports as port_list
from serial import Serial, SerialException
from pprint import pprint
from collections import deque


import gui as graph
import configbuild
import resource
from  metadata.metadata import MetaData
from datastore.data_store import DataStore

#0. Declare resources (globals)
config = None                    # Configuration loaded as dictionary
stop_flag = False				 # Indicates all the threads that the measurement has ended
measurement_ended = False        # The measurement has actually ended
print_data = True				 # Flag that indicates whether data is printed on the screen
no_samples = 4					 # Number of samples being processed
decode_errors = 0 				 # Decoding Errors encountered during data reading
header_text_lines = 7			 # Number of lines to wait before processing the data

#0. Print Header
resource.header()


#1. Load Configuratin file & update globals
config = configbuild.config_load_operations()
header_text_lines = config["Header Lines"]
sep = config["Data Seperator"]
print_data = config["Print Data"]
#2. Open Serial Port
all_ports = list(port_list.comports()) # List of all active ports
try: #Opening the port
	port = Serial(port=config["Port"], baudrate=config["Baud"], timeout=config['Sampling Delay ms']*0.001/2)
	time.sleep(2)
	port_desc = [p.description for p in all_ports if p.device == config["Port"]]
	print(f"Opened port - {config['Port']} - {port_desc[0]}.")
except (SerialException, FileNotFoundError) as e:
	print(f"[ERROR] Port {config['Port']} could not be opened. Exiting!")
	print("○○○ Listing all available ports (change config file appropriately):")
	for port in all_ports:
	    print(f"   → {port}")
	sys.exit(1)

#3. Metadata & Datastore
md = MetaData()
md.add("Fluorimetry", "fluori")
md.set_header("fluorimeter")
md.add_question("What are your samples?")
conc_question = "What concentrations are you using?"
md.add_question(conc_question)
md.collect(username_header=True)
tmp_dir_name = md.file_descriptor()
if tmp_dir_name == "":
	tmp_dir_name = None
ds = DataStore(parent_path=".", session_name=tmp_dir_name, force=(tmp_dir_name==""), test_dirs=False, verbose=True)

data_file_path = ds.new("data.dat")
data_file = open(data_file_path, 'w', buffering = 1)

conc_values = [""]*no_samples
#if conc_question in md.metadata: # User answered the question
#	concs = resource.extract_conc(md.metadata[conc_question])
#	for i in range(len(conc)):
#		if i < no_samples:
#			conc_values[i] = conc[i]



#4. Construct GUI and declare update_fn
gui = graph.GenerateGUI(sub_plots=no_samples)

graphed_points = 0  					# Number of points plotted
updated_lines = 0   					# Number of lines read
sample_id = 0       					# Running counter of sample number
line_change = False                      # New line was detected - one cycle is over

#GUI title generator
def graph_title_gen(sample_no):
	global conc_values, updated_lines, header_text_lines
	conc_str = f"[{conc_values[sample_no]}]"*(conc_values[sample_no] != "")
	title = f"Sample: {sample_no} {conc_str}   |||     Update: {updated_lines-header_text_lines}"
	return title

def numeric_extract(string, time):
	"""
	Returns a list of numeric values from a given string.
	Filters out unwanted characters.
	"""
	global sep, decode_errors
	
	# Sting clean-up
	clean_str = string.replace('\n', '')
	clean_str = clean_str.replace('\r', '')
	
	clean_str = clean_str.lstrip(sep)
	clean_str = clean_str.rstrip(sep)

	# Splitting & filtering of string
	splitted = string.split(sep)
	splitted = [item for item in splitted if resource.is_number(item)] #Filter

	try:
		#Convert into floats
		converted = [float(number) for number in splitted]
		return converted
	except (UnicodeDecodeError, ValueError) as e:
		print(f"Decode Error! @ time elapsed: {time: .3f} s")
		decode_errors = decode_errors + 1

def file_text_generator(list_data, list_time):
	"""
	Creates a uniform seperated [data][sep][time] arangement.
	Warning: Does not check if the lengths of the lists are the same.
	"""

	global sep
	string = ""
	if len(list_data) > 0:
		for i in range(len(list_data)-1):
			string += f"{list_data[i]:.3f}"
			string += str(sep)
			string += f"{list_time[i]:.3f}"
			string += str(sep)

		string += f"{list_data[-1]:.3f}"
		string += sep
		string += f"{list_time[-1]:.3f}"

	return string

def update_fn():
	duration = 0.0 # Duration of arrival - calculated after the `start_time`
	
	# Measurement has ended.
	if stop_flag:
		measurement_ended = True

	
	# Measurement has not ended + Data available on port
	elif port.in_waiting > 0:


		#Read line and record time
		byte_array = port.readline()
		time_now = time.perf_counter()
		
		global sample_id, updated_lines, decode_errors, header_text_lines, graphed_points, line_change
		
		# Line change triggers reset
		if line_change: #Reset
			sample_id = 0
			updated_lines = updated_lines + 1
			line_change = False
		
		# Print Measurement Cycle header
		if sample_id == 0 and updated_lines >= header_text_lines and print_data:
			print(f"○ Measurement cycle - {updated_lines - header_text_lines}")

		try:	
			line = byte_array.decode('ascii')
			global start_time
			duration = time_now - start_time
		
			# Handle header explicitly & return
			if updated_lines < header_text_lines:
				print(line, end='')
				if '\n' in line:
					updated_lines = updated_lines + 1
				return

			# 0. create lists for values and time
			list_values = numeric_extract(line, duration)
			read_values = len(list_values)	
			list_time = [duration]*read_values

			# 1. Write data to file
			data_file.write(file_text_generator(list_values, list_time))
			if line.endswith('\n'): #Line end is detected
				line_change = True
				data_file.write('\n')
			else:
				data_file.write(sep)

			# 2. Print To Terminal
			if print_data:
				for i in range(len(list_values)):
					print(f"→ {list_time[i]: .3f} s  → {list_values[i]}")

			# 3. Update live graph
			for i in range(read_values):
				if sample_id < no_samples:
					gui['data'][sample_id].append(list_values[i])
					gui['time'][sample_id].append(list_time[i])
					gui['canvases'][sample_id].setTitle(title=graph_title_gen(sample_id))
					gui['curves'][sample_id].setData(gui['time'][sample_id], gui['data'][sample_id])	
					graphed_points = graphed_points + 1
				sample_id = sample_id + 1

		# If Ascii conversion fails
		except (UnicodeDecodeError, ValueError) as e:
			print(f"Decode Error! @ time elapsed: {duration:.3f} s")
			decode_errors = decode_errors + 1



#5. Launch thread & start measurement
measurement_sampling_delay = int(config['Sampling Delay ms'])
print("○○○ Launching Live Graph → Close graph window to end measurement.")
measurement_thread = QtCore.QTimer()
measurement_thread.timeout.connect(update_fn)
measurement_thread.start(measurement_sampling_delay)


port.write(str('start\n').encode())
port.flush()
time.sleep(2)

start_datetime = datetime.datetime.now()
md.add("Start Measurement", str(start_datetime))
start_time = time.perf_counter()

gui['app'].exec_() # Launch GUI



###################################
# Measurement
###################################
# ↓ Fall through after window is closed



#6. Stop Procedure
port.write(str('stop\n').encode())
port.flush()
time.sleep(2)

stop_flag = True
stop_datetime = datetime.datetime.now()
md.add("Stop Measurement", str(stop_datetime))
end_time = time.perf_counter()
md.add("Measurement Duration s", (end_time - start_time))



# Close port and files
if measurement_ended: 
	print(f"Measurements have ended. The device will stop after finishing the cycle.")
port.close()
data_file.close()

# Collect additional meta-data
md.add("Data Points", updated_lines)
md.add("Errors", decode_errors)
md.add("Graphed Points", graphed_points)
if config["Export Config to MetaData"]:
	md.add("config", config)
metadata_file_path = ds.new("metadata.yaml")
md.metadata_file(metadata_file_path)
print(f"Metadata exported to: {metadata_file_path}")
print("○○○ Experiment has ended.")

			

# String Preprocessor Functions ↓
def file_preprocessor(string, time):
	"""
	Returns a cleaned string with time point appended that can be placed in a file directly.
	This function breaks when data is bunched: [[data][sep][data][sep]] - only one time point
	is included for all two data points.
	"""

	global sep
	
	# [sep][data][...]__
	if string.startswith(sep):
		string = string.lstrip(sep)

	# [data][\r\n]__
	if string.endswith('\n') or start.endswith('\r'):
		string = string.replace('\n', '')
		string = string.replace('\r', '')
		string = string.rstrip(sep) #Safety

		string = f"{string}{sep}{time}\n"
	
	# [data][sep]__
	elif string.endswith(sep):
		string = f"{string}{time}"

	# [data]__
	else:
		string = f"{string}{sep}{time}"

	return string



