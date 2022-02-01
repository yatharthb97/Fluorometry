"""
Author : *Yatharth Bhasin* (Github → yatharthb97)
License: *MIT open-source license* (https://opensource.org/licenses/mit-license.php)
This piece of software was released on GistHub : https://gist.github.com/yatharthb97/a0b3a2665f065d982e7b0e2b2dd274b0

→ Use `git mv <oldname> <newname>` to properly change the repo folder name to something relevant.

DataStore is utility object that can be used to effectively create and manage a 
directory of files for any generic use case.

"""
import os
from threading import Lock
import time

class DataStore:
    
    """
    Single Layer Data Store
    -----------------------

    Data Store is an object that manages creation of a session directory, 
    and maintains a list of references to file paths in its scope. It also
    has utility functions to create and book-keep files in it.

    • Special directory functions - {test_dirs, force}
    • Low level session name sanatization
    """

    class_lock = Lock() #Global class lock

    def __init__(self, parent_path=".", session_name=None, force=False, test_dirs=False, verbose=False):
        """
        parent_path : The parent directory for the data store
        session_name : Unique name for the session that is used as the directory name 
                       (`None` uses nano-second epoch time by default.)
        force : Force create a directory, by trying a similar name (by appending integers at the 
                end), if a directory with that name is already present.
        test_dirs : Over-ride / Erase a directory, if its name starts with `test. 
                    (Should be used during debug and testing only)
        verbose : Prints a few informative messages.
        """
        session_name = session_name.replace(' ', '_')
        session_name = session_name.replace('-', '_')
        self.name_ = session_name
        self.verbose = verbose
        self.parent_path = os.path.abspath(parent_path)

        if session_name == None:
            if verbose:
                print(f"Using time value as name.")
            self.name_ = str(time.time_ns())

        path = self.DirCreationPolicy(self.parent_path, self.name_, force, test_dirs)
        if path == None:
            raise Exception(f"Data Store could not find a directory for: {self.parent_path} → {self.name_}.")
            
        os.mkdir(path, mode = 0o777)
        self.path = path

        self.files = [] #List of file names

    def session(self):
        """
        Returns the session path.
        """
        return self.path

    def new(self, filename):
        """ 
        Registers and returns a new filepath object. 
        Returns `None` if that path already exists.
        """
        if not filename in self.files:
            self.files.append(filename)
            return os.path.join(self.path, filename)
        else:
            return None 

    def get(self, filename):
        """
        Returns a filename is file is present in the index of the object. Else returns `None`.    
        """
        if filename in self.files:
            return os.path.join(self.path, filename)
        else:
            return None 

    def __repr__(self):
        """ Representation of class. """
        return f"<DataStore obj: {self.path}>"


    def delete_store(self):
        """
        Use with caution.
        """
        from shutil import rmtree
        rmtree(self.path)
        self.path = None
        self.files = []

    
    def DirCreationPolicy(self, parent_path, name_, force=False, test_dirs=False):
        """
        Negotiates a directory path for the data store.
        """

        #Acquire global lock
        self.class_lock.acquire(blocking=True)

            
        path = os.path.join(parent_path, name_)
        path_available = os.path.isdir(path)
        

        if not path_available:
            self.class_lock.release()
            return path
        else:
            if test_dirs == True :
                if name_.startswith("test"):
                       from shutil import rmtree
                       rmtree(path)
                       if self.verbose:
                            print(f"In parent directory `{parent_path}`: ")
                            print(f"DataStore: Over-riding (test) directory `{name_}` which already exists.")
                            self.class_lock.release()
                            return path

            elif force == True:
                MAXTRY = 100
                trycnt = 0
                while trycnt <= MAXTRY:
                    trycnt = trycnt + 1
                    try_path = os.path.join(parent_path, f"{name_}_{trycnt}")
                    if not os.path.isdir(try_path):
                       if self.verbose:
                        print(f"In parent directory `{parent_path}`: ")
                        print(f"DataStore: Force allocated `{name_}_{trycnt}` as `{name_}` already exists.")
                       self.class_lock.release()
                       return try_path
            else:
                if self.verbose:
                    print(f"In parent directory `{parent_path}`: ")
                    print(f"DataStore: Allocation for '{name_}' failed as it already exists.")
                self.class_lock.release()

            # Finally
            return None