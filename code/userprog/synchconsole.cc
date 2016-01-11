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
	char buff[2];
	buff[0] = ch;
	buff[1] = '\0';
	SynchPutString(buff);
}

int SynchConsole::SynchGetChar()
{
	char buff[1];
	SynchGetString(buff, 1);

	return (int)buff[0];
}

void SynchConsole::SynchPutString(const char s[])
{
	mutex->P();
	int i = 0;
	while(s[i] != '\0') {
		console->PutChar(s[i++]);
		writeDone->P();
	}
	mutex->V();
}

void SynchConsole::SynchGetString(char *s, int n)
{
	mutex->P();
	int i;
	for(i = 0; i < n; i++) {
		readAvail->P();
		s[i] = console->GetChar();
		if (s[i] == '\n') {
			s[i+1] = '\0';
			break;
		} else if (s[i] == EOF){
			s[i] = '\0';
			break;
		}
	}
	mutex->V();
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