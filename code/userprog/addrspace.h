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
#include "frameprovider.h"
#include "bitmap.h"

#define NbPageUserThread 2 // Number of pages for a user thread
#define NbPageHeap 10 // Number of pages for dynamic allocation

#define MaxUserThreads 12
#define MaxForkExec 12

#define UserStackSize (MaxUserThreads*PageSize*NbPageUserThread + 2*PageSize)  // increase this as necessary!
#define UserHeapSize NbPageHeap*PageSize

class Semaphore;
class BitMap;

// Structure for keeping track of used IDs
struct ThreadId {
  unsigned int id;
  Semaphore *sem;
  int waited;
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
    int IsJoinableUserThread (unsigned int threadId, unsigned int joinId);

    // Put the thread for threadId in waiting
    void WaitForThread (unsigned int threadId, unsigned int joinId, Semaphore *semJoin);

    /* Return an incremented id that is unused */
    int FindUserThreadId ();

    int Sbrk(unsigned int n);

    Semaphore *semWaitUserThreads;
    Semaphore *semWait;  // Semaphore used for wait a child
    Semaphore *semWaitFromFather;  // Semaphore used for wait a child
    Semaphore **tabSemUser;

  private:
      TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual 
    // address space

    ThreadId **threadList;  // Pointer to the beginning of the ID list
    BitMap *bitmap;
    unsigned int nbThreads; // Number of running user threads
    Semaphore *mutex; // Mutex
    unsigned int brk;
};

#endif // ADDRSPACE_H
