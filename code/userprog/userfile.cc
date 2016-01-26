#include "userthread.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"
#include "thread.h"
#include "addrspace.h"
#include "errorno.h"
#include "userfile.h"
/*
void do_Create(char *name){
	
	
	if(fileSystem->Create(name, 0))
		PutString("Fichier créé\n");
	else
		PutString("Erreur lors de la création du fichier\n");
	
	//synchDisk
}

OpenFileId do_Open (char *name){
	
	return fileSystem->Open(name);
	
}

void do_Write(char *buffer, int size, OpenFileId id){
	
	
	
}

int do_Read(char *buffer, int size, OpenFileId id){
	
	
	
}

void do_Close(OpenFileId id){
	
	
	
}
/*
void createDir(char *nameDir){
	
	
	
}

void destroyDir(char *nameDir){
	//for destroying a directory, it must be empty.
	
}

void changeDir(char *nameDir){
	
	
}

void printDir(char *nameDir){
	
}
*/
