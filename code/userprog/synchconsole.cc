#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
	mutex = new Semaphore("Synchronisation", 1);
}

SynchConsole::~SynchConsole()
{
	delete mutex;
	delete console;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	mutex->P();
	console->PutChar(ch);
	writeDone->P();
	mutex->V();
}

int SynchConsole::SynchGetChar()
{
	mutex->P();
	readAvail->P();
	int ret = (int)console->GetChar();
	mutex->V();

	return ret;
}

void SynchConsole::SynchPutString(const char s[])
{
	int i = 0;
	while(s[i] != '\0') {
		SynchPutChar(s[i++]);
	} 
}

void SynchConsole::SynchGetString(char *s, int n)
{
	int i;
	for(i = 0; i < n; i++) {
		s[i] = (char)SynchGetChar();
		if (s[i] == '\n') {
			s[i+1] = '\0';
			break;
		} else if (s[i] == EOF){
			s[i] = '\0';
			break;
		}
	}
}

void SynchConsole::SynchPutInt(int n)
{
	char buff[MAX_STRING_SIZE];
	sprintf(buff, "%d", n);
	SynchPutString(buff);
}

int SynchConsole::SynchGetInt() {
	int ret;
	char buff[MAX_STRING_SIZE];
	SynchGetString(buff, MAX_STRING_SIZE-1);
	sscanf(buff, "%d", &ret);
	return ret;
}