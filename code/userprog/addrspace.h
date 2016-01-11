// addrspace.h 
//      Data structures to keep track of executing user programs 
//      (address spaces).
//
//      For now, we don't keep any information about address spaces.
//      The user level CPU state is saved and restored in the thread
//      executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "synch.h"

#define UserStackSize		1024	// increase this as necessary!

#define NbPageUserThread 2 // Number of pages for a user thread

#define MaxUserThreads 12

class Semaphore;

// Structure for keeping track of used IDs
struct ThreadId {
  unsigned int id;
  unsigned int idSpace;
  Semaphore *sem;
  int waited;
  struct ThreadId *next;
};

class AddrSpace
{
  public:
    AddrSpace (OpenFile * executable);	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace ();		// De-allocate an address space

    void InitRegisters ();	// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState ();		// Save/restore address space-specific
    void RestoreState ();	// info on a context switch

    // Add the user thread to the ID list and increment nbThread
    // Return the adress of a stack space for the user thread
    // or -1 if there isn't any space available
    // This function set the thread's ID space
    int FindUserThreadSpace (unsigned int *threadIdSpace, unsigned int threadId);

    // Remove the user thread to the ID list and decrement nbThread and returns the semaphore waiting for it
    Semaphore* RemoveUserThread (unsigned int threadIdSpace);
	
    // Return the number of user threads running
    int GetNbUserThreads ();

    // Return -1 if the thread isn't in the ID list, 0 otherwise
    int ExistsUserThread (unsigned int threadId);

    void WaitForThread (unsigned int threadId, Semaphore *semJoin);

    /* Return an incremented id that is unused */
    int FindUserThreadId ();

    Semaphore *semWaitUserThreads;

  private:
      TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual 
    // address space

    struct ThreadId *IDList;  // Pointer to the beginning of the ID list
    unsigned int nbThreads; // Number of running user threads
    Semaphore *mutex; // Mutex
};

#endif // ADDRSPACE_H
