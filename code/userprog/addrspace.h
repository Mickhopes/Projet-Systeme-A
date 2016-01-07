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

#define MaxUserThreads  4 // TODO: calculer le nombre de threads user possible. genre (taille de l'espace d'adressage - taille main) / UserStackSize

// Structure for keeping track of used IDs
struct ThreadId {
  unsigned int id;
  struct ThreadId *next;
};

class Semaphore;

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
    // This function set the thread's ID
    int FindUserThreadSpace (unsigned int *threadId);

    // Remove the user thread to the ID list and decrement nbThread
    void RemoveUserThread (unsigned int threadId);
	
	// Return the number of user threads running
	int GetNbUserThreads ();

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
