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

import gui as graph
import configbuild
import resource
from  metadata.metadata import MetaData
from datastore.data_store import DataStore

#0. Declare resources
config = None
stop_flag = False
measurement_ended = False
print_data = True
no_samples = 4
decode_errors = 0
header_text_lines = 4

# Print Header
resource.header()


#1. Load Configuratin file
config = configbuild.config_load_operations()


#2. Metadata & Datastore
md = MetaData()
md.add("Fluorimetry", "fluori")
md.set_header("fluorimeter")
md.add_question("What are your samples?")
md.add_question("What concentrations are you using?")
md.collect(username_header=True)
tmp_dir_name = md.file_descriptor()
if tmp_dir_name == "":
	tmp_dir_name = None
ds = DataStore(parent_path=".", session_name=tmp_dir_name, force=(md.file_descriptor()==""), test_dirs=False, verbose=True)
data_file_path = ds.new("data.dat")
data_file = open(data_file_path, 'w', buffering = 1)





#3. Open Serial Port
all_ports = list(port_list.comports())
try:
	port = Serial(port=config["Port"], baudrate=config["Baud"], timeout=config['Sampling Delay ms']*0.001/2)
	time.sleep(2)
	port_desc = [p.description for p in all_ports if p.device == config["Port"]]
	print(f"Opened port - {config['Port']} - {port_desc[0]}.")
except (SerialException, FileNotFoundError) as e:
	print(f"[ERROR] Port {config['Port']} could not be opened. Exiting!")
	print("••• Listing all available ports (change config file appropriately):")
	for port in all_ports:
	    print(f"   → {port}")
	data_file.close()
	ds.delete_store()
	sys.exit(1)






#4. Construct GUI and declare update_fn
gui = graph.GenerateGUI(composite_plot=config['Composite Plot'], sub_plots=no_samples)

graphed_points = 0
updated_lines = 0
sample_id = 0
numeric_data = [0.0]*(no_samples+1)
from collections import deque

def update_fn():
	duration = 0.0
	
	# Measurement has ended.
	if stop_flag:
		measurement_ended = True

	
	# Measurement has not ended + Data available on port
	elif port.in_waiting > 0:


		#Read line and record time
		x = port.readline()
		time_now = time.perf_counter()
		
		try:
			global sample_id, updated_lines, numeric_data, decode_errors, header_text_lines, graphed_points
			
			if sample_id > no_samples:
				sample_id = 0
				updated_lines = updated_lines + 1
				numeric_data = [0.0] * (no_samples+1)


			if sample_id == 0 and updated_lines >= header_text_lines and print_data:
				print(f"• Measurement cycle - {updated_lines - header_text_lines}")


			line = x.decode('ascii')
			global start_time
			duration = time_now - start_time
		
			# Handle header explicitly
			if updated_lines < header_text_lines:
				print(line, end='')
				if '\n' in line:
					updated_lines = updated_lines + 1
				return

			#Save to file
			save_line = f"{duration:.3f}\t{line}"
			data_file.write(save_line)
			if not line.endswith('\n'):
				data_file.write('\t')

			# Print To Terminal
			if print_data:
				print(f"{duration: .3f} s  → {line}")


			# Clean up text and Convert to floats
			
			### Clean-up
			clean_str = line.rstrip('\n')
			clean_str = clean_str.rstrip('\r')
			clean_str = clean_str.rstrip('\t')
			clean_str = clean_str.lstrip('\t')

			# Splitting & Filter
			split_str = clean_str.split('\t') #Split String into distinct numbers
			split_str = [item for item in split_str if item.isnumeric()]
			read_samples = len(split_str)
				
			try:

				converted = [float(number) for number in split_str] #Convert into floats
				j = 0
				for i in range(sample_id, sample_id+read_samples):
					numeric_data[i] = converted[j]
					j = j +1
					if i < no_samples:
						gui['data'][i].append(numeric_data[i])
						gui['time'][i].append(duration)
						gui['canvases'][i].setTitle(title=f"Sample - {i}     |||     Update - {updated_lines-no_samples}")
						gui['curves'][i].setData(gui['time'][i], gui['data'][i])
					
				
				sample_id = sample_id + read_samples
				graphed_points = graphed_points + read_samples

			#If Conversion to integers fails
			except (UnicodeDecodeError, ValueError) as e:
				print(f"Decode Error! @ time elapsed: {duration: .3f} s")
				decode_errors = decode_errors + 1

		except (UnicodeDecodeError, ValueError) as e:
			print(f"Decode Error! @ time elapsed: {duration: .3f} s")
			decode_errors = decode_errors + 1



#5. Launch thread & start measurement
measurement_sampling_delay = int(config['Sampling Delay ms'])
print("••• Launching Live Graph → Close graph window to end measurement.")
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
#while measurement_ended != True: # Blook till the last measurement has ended
#	time.sleep(100)
port.close()
data_file.close()

md.add("Data Points", updated_lines)
md.add("Errors", decode_errors)
metadata_file_path = ds.new("metadata.yaml")
md.metadata_file(metadata_file_path)
print(f"Metadata exported to: {metadata_file_path}")
print("••• Experiment has ended.")

				