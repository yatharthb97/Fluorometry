#!/usr/bin/env python3
#!/usr/bin/env python

"""
Metadata aggregator
Object that is specialized  to collect, store, and export metadata for any generic application.

Author : *Yatharth Bhasin* (Github → yatharthb97)
License: *MIT open-source license* (https://opensource.org/licenses/mit-license.php)
This piece of software was released on GistHub : TODO
"""

def DescriptorGenerator():
	"""
	Generator function that sequentially returns 
	`keys` for metadatum if none is given by the user.
	"""

	i = 0
	while True:
		i = i + 1
		yield 'descriptor-' + str(i)


class MetaData:

	default_header = "| meta-data collector >" # Default header
	default_no_input_msg = "Okay! Keep your secrets!" # Default no input message


	def __init__(self):
		"""
		Constructor.
		"""
		self.metadata = {}  # Stores metadata
		self.datetime = {}  # Stores datetime metadata
		self.questions = [] # List of extra questions to be asked to the user
		self.include_weekday = True # Include day explicitly in metadata construct
		self.fd_restrict = None # Use finite number of meta-date entities for generation of file descriptor 
		self.collected = False #Indicates whether the collection cycle is complete

		self.header = self.default_header  # Header to be used with the
		self.no_input_msg = self.default_no_input_msg # Message when no input is given
		self.no_input_str = f"{self.header} {self.no_input_msg}" # Header + No input message

		self.descriptor = DescriptorGenerator() # Instance of descriptor generator

	def set_header(self, hstring="", name=None):
		"""
		Changes the header string to passed `hstring`.
		"""
		if hstring == "":
			hstring = self.header.replace("| ", "")
			hstring = hstring.replace(" >", "")
		suffix = ""
		if name != None:
			suffix = f"{name}'s "
		self.header = f"| {suffix}{hstring} >"
		self.no_input_str = f"{self.header} {self.no_input_msg}"

	
	def collect(self, user_interactive=True, username_header=False):
		"""
		Collects the metadata in a user-nteractive Q & A.
		
		user_interactive (optional) : Interact with user to collect information (blocking).
		username_header (optional) : Change the header to include the user's name.
		"""

		# Collect date and time
		import datetime
		dt = datetime.datetime.now()
		self.datetime["year"] = dt.year
		self.datetime["month"] = dt.month
		self.datetime["day"] = dt.day
		self.datetime["hour"] = dt.hour
		self.datetime["min"] = dt.minute
		self.datetime["weekday"] = dt.strftime('%A')
		self.datetime["rep"] = str(dt)
		self.metadata["date"] = f"{self.datetime['day']}-{self.datetime['month']}-{self.datetime['year']}"
		
		if self.include_weekday:
			self.metadata["weekday"] = self.datetime["weekday"][0:3]

		if user_interactive:
			# Collect user name
			user = input(f"{self.header} What is your name? → ")
			if user != "":
				if username_header:
					self.set_header(name=user)
				print(f"{self.header} Hello! {user}.")
				self.metadata['user'] = user
			else:
				print(self.no_input_str)

			# Ask pre-decided Q & A
			for que in self.questions:
				ans = input(f"{self.header} {que} → ")
				if ans != "":
					print(f"{self.header} recorded: {ans}")
					self.metadata[que] = ans
				else:
					print(self.no_input_str)
				

			#Collect additional experiment metadata
			meta_list_str = input(f"{self.header} [optional] Describe your setup with a few key words (comma-seperated): ")
			if meta_list_str != "":
				meta_list = meta_list_str.replace(" ", "").split(",")
				print(f"{self.header} Info collected: {meta_list}")
				for i, data in enumerate(meta_list):
					self.metadata[next(self.descriptor)] = meta_list[i]
			else:
				print(self.no_input_str)

		# Collection is complete
		self.collected = True

	def file_descriptor(self):
		"""
		Returns a string constructed from meta-deta that functionally describes a directory name.
		"""

		if self.collected != True:
			print(f"{self.header} Warning: No meta-data collection has occured. Generating file descriptor anyway.")

		# String generation from list
		fd_str = ""
		md_list = list(self.metadata.values())
		
		# Finite elements
		if self.fd_restrict != None:
			restrict = (self.fd_restrict <= len(md_list)) * self.fd_restrict + (self.fd_restrict > len(md_list)) * len(md_list)
			md_list = md_list[:restrict]

		for md in md_list:
			fd_str += str(md)
			fd_str += '__'

		fd_str = fd_str[:-2]


		#String sanatization
		fd_str = fd_str.replace(".", "•") #Period
		fd_str = fd_str.replace(" ", "_") #Space

		# generic sanatization
		for char in "<>/{}[]~`\\":
			fd_str = fd_str.replace(char, '#')


		return fd_str


	def metadata_file(self, filename, json=False, yaml=False):
		"""
		Generates a metadata file for the given filename. File extension also determines save mode.

		json (optional) : Forces creation of a json file.
		yml (optional) : Forces the creation of a yml file.
		Exception is raised if both are `True`. Options override file extenstion.
		"""

		if self.collected != True:
			print(f"{self.header} Warning: No meta-data collection has occured. Generating meta-data file anyway.")

		mode = "ascii"

		if json and yaml:
			raise Exception("For meta-data file generation - conflicting ioptions passed.")

		if filename.endswith(".json"):
			mode = "json"

		elif filename.endswith(".yaml"):
			mode = "yaml"

		elif json:
			mode = "json"

		elif yaml:
			mode = "yaml"


		if mode != "ascii" and not filename.endswith(f".{mode}"):
			filename += f".{mode}"

		metadata = self.metadata.copy()
		if '-' in metadata:
			metadata.pop('-', None)

		data = {"metadata" : metadata, "time" : self.datetime}

		if mode == "ascii":
			with open(filename, 'w') as file:
				data_str = str(data)
				file.write(data_str)

		if mode == "json":
			import json
			with open(filename, 'w') as file:
			    json.dump(data, file,  indent=4)
			    

		if mode == "yaml":
			import yaml
			with open(filename, 'w') as file:
			    yaml.dump(data, file, indent=4)

	
	def add_question(self, q_str):
		"""
		Add a question that will be asked to the user 
		during `metadata.collect(user_interactive=True)` call.
		"""
		if q_str != "":
			self.questions.append(q_str)

	
	def add(self, key, value):
		"""
		Add key and value pair to metadata.
		"""
		if value != "":
			if key == "":
				key = next(self.descriptor)
			self.metadata[key] = value

	def reset(self):
		"""
		Resets the metadata structure.
		"""
		self.header = self.default_header
		self.no_input_msg =  self.default_no_input_msg
		self.metadata = {}
		self.datetime = {}
		self.questions = []
		self.collected = False
		self.include_weekday = True
		self.fd_restrict = None
		self.descriptor = DescriptorGenerator()

