// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"
#include "directory.h"


#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {
  public:
    FileSystem(bool format) {}

    bool Create(char *name, int initialSize) { 
	int fileDescriptor = OpenForWrite(name);

	if (fileDescriptor == -1) return FALSE;
	Close(fileDescriptor); 
	return TRUE; 
	}
	
	bool CreateDirectory(char *name);

    OpenFile* Open(char *name) {
	  int fileDescriptor = OpenForReadWrite(name, FALSE);

	  if (fileDescriptor == -1) return NULL;
	  return new OpenFile(fileDescriptor);
      }

    bool Remove(char *name) { return Unlink(name) == 0; }

};

#else // FILESYS
class FileSystem {
  public:
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.
	
    bool Create(char *name, int initialSize);  	
					// Create a file (UNIX creat)
    
    int CutFullNameInTabName(char *fullName, char **tabFullName);
/*
	* Research
	* this function search the father directory of the new drectory 
	* return father directory if is find
	* else return NULL
	*
	* name is the name of the new directory
	*/
    Directory *Research(char *name, char *dirNamee);

    /*
	* createDirectory
	* call createDir when find the father of the directory where is createDir
	*
	* name is the relative or absolute name of the new directory
	*/			
	int CreateDirectory(char *name);
	
    OpenFile* Open(char *name); 	// Open a file (UNIX open)

    bool Remove(char *name); 	// Delete a file (UNIX unlink)
    
    
    
    

	/*
	*List all file and direstory in file system
	*/
    void ListAll();		
    
    /*
    * ListName
    * this function list a directory named name
    * if it's a file and not a directory just write the name of file
    * if the name unexist the function do nothing
    */
	void ListName(char * name);
     
	/*
	* GetCurrentDirectory
	* return a current directory;
	*/				
	Directory *GetCurrentDirectory();

    void Print();			// List all the files and their contents

  private:
	OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
	OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file
	

	
					
	/*
	* CreateDir
	* this function create a directory named name in the current directory
	* if one error is raise in the function return -1
	* when the function return -1 errorno is update and the creation is a faillure
	*
	* name is the name of the new directory
	*/				
	int CreateDir(char *name);
};

#endif // FILESYS

#endif // FS_H
