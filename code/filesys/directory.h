// directory.h 
//	Data structures to manage a UNIX-like directory of file names.
// 
//      A directory is a table of pairs: <file name, sector #>,
//	giving the name of each file in the directory, and 
//	where to find its file header (the data structure describing
//	where to find the file's data blocks) on disk.
//
//      We assume mutual exclusion is provided by the caller.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "openfile.h"
#include "errorno.h"

#define fileNameMaxLen 		9	// for simplicity, we assume 
					// file names are <= 9 characters long
#define directoryNameMaxLen 100 // each name of directory is same size of the fileNameMaxLen

#define DepthMaxFile 10
					
#define specialEntry 2

#define currentDirectory 0
#define fatherDirectory 1

#define nameCurrentDirectory "."
#define nameFatherDirectory ".."

#define NumDirEntries	10

// The following class defines a "directory entry", representing a file
// in the directory.  Each entry gives the name of the file, and where
// the file's header is to be found on disk.
//
// Internal data structures kept public so that Directory operations can
// access them directly.

class DirectoryEntry {
  public:
    bool inUse;				// Is this directory entry in use?
    int sector;				// Location on disk to find the 
					//   FileHeader for this file 
    char name[fileNameMaxLen + 1];	// Text name for file, with +1 for 
					// the trailing '\0'
};



	

// The following class defines a UNIX-like "directory".  Each entry in
// the directory describes a file, and where to find it on disk.
//
// The directory data structure can be stored in memory, or on disk.
// When it is on disk, it is stored as a regular Nachos file.
//
// The constructor initializes a directory structure in memory; the
// FetchFrom/WriteBack operations shuffle the directory information
// from/to disk. 

class Directory {
  public:
	/*
	* Initialize an empty directory
	* size is the number of entries in directory(2 reserved for "." and "..")
	* nameDirectory is the name of the directory
	* sec is a pointer on header of the current directory 
	* fatherSec is a pointer on header of the father directory 
	*/
    Directory(int size, int sec, int fatherSec);
    Directory(int size);
    
    ~Directory();			// De-allocate the directory

    void FetchFrom(OpenFile *file);  	// Init directory contents from disk
    void WriteBack(OpenFile *file);	// Write modifications to 
					// directory contents back to disk
					

	/*
	* FindSectorWithName
	* return the sector corresponding to a name in the current directory
	* if the name isn't exist in this directory return -1
	*/
    int FindSectorWithName(char *name);		
    
	/*
	* FindNameWithSector
	* return the name corresponding to a sector in the current directory
	* if the sector isn't exist in this directory return NULL
	*/				
	char *FindNameWithSector(int sector);
	
	/*
	* FindIndexWithName
	* return the position corresponding to a name in the current directory
	* if the name isn't exist in this directory return -1
	*/				
	int FindIndexWithName(char *name);

	/*
	 * Add a file into a directory
	 * 
	 * name is the name of new File
	 * newsector  is the sector where the file is save
	*/
    bool AddFile(char *name, int newSector);  // Add a file name into the directory
    /*
     * Add a sub-directory into the directory
     * 
     * name is the name of new directory
     * newsector  is the sector where the directory is save
    */
    bool AddDirectory(char * name, int newSector); //Add a directory into a directory
	
	/*
	 * Add calls functions AddFile or AddDirectory
	 * According to target
	 * before calling function add 	verify if Directory Is Full 
	 * 								verify if name exist 
	 *								verify the lenght of name
	 * returns TRUE if successful, FALSE otherwise 
	 * errorno is updated
	 *
	 * name is the name of directory or file
	 * newSector is the sector where the file (or directory) is save
	 * isDirectory indicated the type of target(0 for directory and 1 for directory)
	 */
    bool Add(char *name, int newSector, int isDirectory);
    


	/*
	 * remove file with name = nameFile in directory
	 * return TRUE if success
	 * FALSE else
	 * when return FALSE errorno is updated
	*/
    bool RemoveFile(char *nameFi);
    
    /*
	 * remove directory with name = nameDir in directory
	 * return TRUE if success
	 * FALSE else
	 * when return FALSE errorno is updated
	*/
    bool RemoveDirectory(char *nameDir);
    
    /*
     * function remove call RemoveFile or removeDirectory
     * according to target's type
     * same return as for the two called functions 
    */
    bool Remove(char *name);
    
    
    /*
     * DirectoryIsEmpty returns TRUE if the directory is empty
     * else FALSE 
    */
    bool DirectoryIsEmpty();
    
    /*
     * DirectoryIsEmpty returns TRUE if the directory is full
     * else FALSE 
    */
    bool DirectoryIsFull();
    
    /*
     * DirectoryIs returns TRUE if the directory is root of file system
     * else FALSE 
    */
    bool DirectoryIsRoot();
    
    /*
	* GetNameDirectory return the name of current directory
    */
    char *GetNameDirectory();  
    
    /*
    * GetTable
    * return table
    */
    DirectoryEntry *GetTable();

    void List();	// Print the names of all the files
					//  in the directory
    void Print();			// Verbose print of the contents
					//  of the directory -- all the file
					//  names and their contents.**
	
	

  private:
  
  
    int tableSize;			// Number of directory entries
    DirectoryEntry *table;		// Table of pairs: 
					// <file name, file header location> 
	
    int FindIndex(char *name);	
		
};

#endif // DIRECTORY_H
