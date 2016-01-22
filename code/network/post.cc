// post.cc 
// 	Routines to deliver incoming network messages to the correct
//	"address" -- a mailbox, or a holding area for incoming messages.
//	This module operates just like the US postal service (in other
//	words, it works, but it's slow, and you can't really be sure if
//	your mail really got through!).
//
//	Note that once we prepend the MailHdr to the outgoing message data,
//	the combination (MailHdr plus data) looks like "data" to the Network 
//	device.
//
// 	The implementation synchronizes incoming messages with threads
//	waiting for those messages.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "post.h"
#include "system.h"

#include <strings.h> /* for bzero */

MailHeader::MailHeader()
{
    last = 1;
    sequence = 0;
}

//----------------------------------------------------------------------
// Mail::Mail
//      Initialize a single mail message, by concatenating the headers to
//	the data.
//
//	"pktH" -- source, destination machine ID's
//	"mailH" -- source, destination mailbox ID's
//	"data" -- payload data
//----------------------------------------------------------------------

Mail::Mail(PacketHeader pktH, MailHeader mailH, char *msgData)
{
    ASSERT(mailH.length <= MaxMailSize);

    pktHdr = pktH;
    mailHdr = mailH;
    bcopy(msgData, data, mailHdr.length);
}

//----------------------------------------------------------------------
// MailBox::MailBox
//      Initialize a single mail box within the post office, so that it
//	can receive incoming messages.
//
//	Just initialize a list of messages, representing the mailbox.
//----------------------------------------------------------------------


MailBox::MailBox()
{ 
    messages = new SynchList(); 
}

//----------------------------------------------------------------------
// MailBox::~MailBox
//      De-allocate a single mail box within the post office.
//
//	Just delete the mailbox, and throw away all the queued messages 
//	in the mailbox.
//----------------------------------------------------------------------

MailBox::~MailBox()
{ 
    delete messages; 
}

//----------------------------------------------------------------------
// PrintHeader
// 	Print the message header -- the destination machine ID and mailbox
//	#, source machine ID and mailbox #, and message length.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//----------------------------------------------------------------------

static void 
PrintHeader(PacketHeader pktHdr, MailHeader mailHdr)
{
    printf("From (%d, %d) to (%d, %d) bytes %d\n",
    	    pktHdr.from, mailHdr.from, pktHdr.to, mailHdr.to, mailHdr.length);
}

//----------------------------------------------------------------------
// MailBox::Put
// 	Add a message to the mailbox.  If anyone is waiting for message
//	arrival, wake them up!
//
//	We need to reconstruct the Mail message (by concatenating the headers
//	to the data), to simplify queueing the message on the SynchList.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//	"data" -- payload message data
//----------------------------------------------------------------------

void 
MailBox::Put(PacketHeader pktHdr, MailHeader mailHdr, char *data)
{ 
    Mail *mail = new Mail(pktHdr, mailHdr, data); 

    messages->Append((void *)mail);	// put on the end of the list of 
					// arrived messages, and wake up 
					// any waiters
}

//----------------------------------------------------------------------
// MailBox::Get
// 	Get a message from a mailbox, parsing it into the packet header,
//	mailbox header, and data. 
//
//	The calling thread waits if there are no messages in the mailbox.
//
//	"pktHdr" -- address to put: source, destination machine ID's
//	"mailHdr" -- address to put: source, destination mailbox ID's
//	"data" -- address to put: payload message data
//----------------------------------------------------------------------

void 
MailBox::Get(PacketHeader *pktHdr, MailHeader *mailHdr, char *data) 
{ 
    DEBUG('n', "Waiting for mail in mailbox\n");
    DEBUG('r', "Avant enlevement liste\n");
    Mail *mail = (Mail *) messages->Remove();	// remove message from list;
						// will wait if list is empty
    DEBUG('r', "Apres enlevement liste\n");

    *pktHdr = mail->pktHdr;
    *mailHdr = mail->mailHdr;
    if (DebugIsEnabled('n')) {
	printf("Got mail from mailbox: ");
	PrintHeader(*pktHdr, *mailHdr);
    }
    bcopy(mail->data, data, mail->mailHdr.length);
					// copy the message data into
					// the caller's buffer
    delete mail;			// we've copied out the stuff we
					// need, we can now discard the message
}

//----------------------------------------------------------------------
// PostalHelper, ReadAvail, WriteDone
// 	Dummy functions because C++ can't indirectly invoke member functions
//	The first is forked as part of the "postal worker thread; the
//	later two are called by the network interrupt handler.
//
//	"arg" -- pointer to the Post Office managing the Network
//----------------------------------------------------------------------

static void PostalHelper(int arg)
{ PostOffice* po = (PostOffice *) arg; po->PostalDelivery(); }
static void ReadAvail(int arg)
{ PostOffice* po = (PostOffice *) arg; po->IncomingPacket(); }
static void WriteDone(int arg)
{ PostOffice* po = (PostOffice *) arg; po->PacketSent(); }

//----------------------------------------------------------------------
// PostOffice::PostOffice
// 	Initialize a post office as a collection of mailboxes.
//	Also initialize the network device, to allow post offices
//	on different machines to deliver messages to one another.
//
//      We use a separate thread "the postal worker" to wait for messages 
//	to arrive, and deliver them to the correct mailbox.  Note that
//	delivering messages to the mailboxes can't be done directly
//	by the interrupt handlers, because it requires a Lock.
//
//	"addr" is this machine's network ID 
//	"reliability" is the probability that a network packet will
//	  be delivered (e.g., reliability = 1 means the network never
//	  drops any packets; reliability = 0 means the network never
//	  delivers any packets)
//	"nBoxes" is the number of mail boxes in this Post Office
//----------------------------------------------------------------------

PostOffice::PostOffice(NetworkAddress addr, double reliability, int nBoxes)
{
// First, initialize the synchronization with the interrupt handlers
    messageAvailable = new Semaphore("message available", 0);
    messageSent = new Semaphore("message sent", 0);
    sendLock = new Lock("message send lock");
    ackLock = new Lock("ack table lock");

// Second, initialize the mailboxes
    netAddr = addr; 
    numBoxes = nBoxes;
    boxes = new MailBox[nBoxes];
    ackTable = new unsigned int*[nBoxes];
    ackNumber = new unsigned int[nBoxes];
    ackSem = new Semaphore*[nBoxes];

    for(int i = 0; i < nBoxes; i++) {
        ackTable[i] = new unsigned int[NbAckTable];

        for(int j = 0; j < NbAckTable; j++) {
            ackTable[i][j] = 0;
        }

        ackNumber[i] = 0;
        ackSem[i] = new Semaphore("Semaphore of boxes", NbAckTable);
    }

// Third, initialize the network; tell it which interrupt handlers to call
    network = new Network(addr, reliability, ReadAvail, WriteDone, (int) this);


// Finally, create a thread whose sole job is to wait for incoming messages,
//   and put them in the right mailbox. 
    Thread *t = new Thread("postal worker");

    t->Fork(PostalHelper, (int) this);
}

//----------------------------------------------------------------------
// PostOffice::~PostOffice
// 	De-allocate the post office data structures.
//----------------------------------------------------------------------

PostOffice::~PostOffice()
{
    delete network;
    delete [] ackNumber;
    for(int i = 0; i < numBoxes; i++) {
        delete [] ackTable[i];
        delete ackSem[i];
    }
    delete [] ackTable;
    delete [] ackSem;
    delete [] boxes;
    delete messageAvailable;
    delete messageSent;
    delete ackLock;
    delete sendLock;
}

//----------------------------------------------------------------------
// PostOffice::PostalDelivery
// 	Wait for incoming messages, and put them in the right mailbox.
//
//      Incoming messages have had the PacketHeader stripped off,
//	but the MailHeader is still tacked on the front of the data.
//----------------------------------------------------------------------

void
PostOffice::PostalDelivery()
{
    PacketHeader pktHdr;
    MailHeader mailHdr, oldMail;
    oldMail.ack = 9999;
    oldMail.from = 9999;
    char *buffer = new char[MaxPacketSize];
    DEBUG('r', "PostalDelivery commence\n");
    for (;;) {
        // first, wait for a message
        messageAvailable->P();
        pktHdr = network->Receive(buffer);

        mailHdr = *(MailHeader *)buffer;
        if (DebugIsEnabled('n')) {
	       printf("Putting mail into mailbox: ");
	       PrintHeader(pktHdr, mailHdr);
        }

    	// check that arriving message is legal!
    	ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    	ASSERT(mailHdr.length <= MaxMailSize);

        DEBUG('r', "PostalDelivery : %d a recu un message : mailHdr.isAck = %d\n", netAddr, mailHdr.isAck);
        if (mailHdr.isAck) {
            DEBUG('r', "Postal delivery a detecté un ACK a destination de %d par %d\n", netAddr, mailHdr.ack);
            ackLock->Acquire();
            ackTable[mailHdr.to][mailHdr.ack % NbAckTable] = 2;
            ackLock->Release();
        } else {
            if (!(oldMail.from == mailHdr.from && oldMail.ack == mailHdr.ack)) {
                oldMail.from = mailHdr.from;
                oldMail.ack = mailHdr.ack;

                // put into mailbox
                boxes[mailHdr.to].Put(pktHdr, mailHdr, buffer + sizeof(MailHeader));
                DEBUG('r', "PostalDelivery : %d envoie un ack num %d à %d sur la boite %d\n", netAddr, mailHdr.ack, pktHdr.from, mailHdr.from);
            }

            SendAck(pktHdr, mailHdr);
        }
    }
    DEBUG('r', "PostalDelivery fini\n");
}

//----------------------------------------------------------------------
// PostOffice::Send
// 	Concatenate the MailHeader to the front of the data, and pass 
//	the result to the Network for delivery to the destination machine.
//
//	Note that the MailHeader + data looks just like normal payload
//	data to the Network.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//	"data" -- payload message data
//----------------------------------------------------------------------

void
PostOffice::Send(PacketHeader pktHdr, MailHeader mailHdr, const char* data)
{
    char* buffer = new char[MaxPacketSize];	// space to hold concatenated
						// mailHdr + data

    if (DebugIsEnabled('n')) {
	printf("Post send: ");
	PrintHeader(pktHdr, mailHdr);
    }
    
    ASSERT(mailHdr.length <= MaxMailSize);
    ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    
    // fill in pktHdr, for the Network layer
    pktHdr.from = netAddr;
    pktHdr.length = mailHdr.length + sizeof(MailHeader);

    // concatenate MailHeader and data
    bcopy(&mailHdr, buffer, sizeof(MailHeader));
    bcopy(data, buffer + sizeof(MailHeader), mailHdr.length);

    sendLock->Acquire();   		// only one message can be sent
					// to the network at any one time
    DEBUG('r', "Send : pktHdr.to = %d\n", pktHdr.to);
    DEBUG('r', "Send : mailHdr.to = %d\n", mailHdr.to);
    DEBUG('r', "Send : mailHdr.from = %d\n", mailHdr.from);
    DEBUG('r', "Send : data = %s\n", data);
    DEBUG('r', "Send : isAck = %d\n", mailHdr.isAck);
    network->Send(pktHdr, buffer);
    messageSent->P();			// wait for interrupt to tell us
					// ok to send the next message
    sendLock->Release();

    delete [] buffer;			// we've sent the message, so
					// we can delete our buffer
}

//----------------------------------------------------------------------
// PostOffice::SendReliable
//  Send the packet using Send() and then wait for an ack answer.
//  If the ack has not been received, it re-send the packet.
//  The packet is reemitted a predefined number of times.
//----------------------------------------------------------------------

void
PostOffice::SendReliable(PacketHeader pktHdr, MailHeader mailHdr, const char* data)
{
    // We wait for a place in our reception box
    ackSem[mailHdr.from]->P();

    // We set the ack index that we will wait in the ackTable
    mailHdr.isAck = 0;
    FindAck(&mailHdr);
    DEBUG('r', "Pour %d, FdindAck a trouvé un num dispo : %d\n", netAddr, mailHdr.ack);

    // We resend until we receive the ack
    int i;
    for(i = 0; i < MAXREEMISSIONS; i++) {
        // We send the packet
        DEBUG('r', "%d envoie un paquet à %d tentative %d\n", netAddr, pktHdr.to, i+1);
        Send(pktHdr, mailHdr, data);

        // We wait a certain amount of time, see TEMPO
        currentThread->Sleep(TEMPO);

        // Then we check if the ack has arrived
        ackLock->Acquire();
        if (ackTable[mailHdr.from][mailHdr.ack] == 2) {
            DEBUG('r', "%d a recu un Ack car la case est a 2\n", netAddr);
            ackLock->Release();
            break;
        }
        ackLock->Release();
    }

    if (i == MAXREEMISSIONS)
        DEBUG('r', "Envoi impossible du message de %d à %d\n", netAddr, pktHdr.to);

    // Then we reset our entry in the ackTable
    ackLock->Acquire();
    ackTable[mailHdr.from][mailHdr.ack] = 0;
    ackSem[mailHdr.from]->V();
    ackLock->Release();
}

//----------------------------------------------------------------------
// PostOffice::SendAck
//  Send an ack to the machine which sent us a packet
//----------------------------------------------------------------------

void
PostOffice::SendAck(PacketHeader pktHdr, MailHeader mailHdr)
{
    PacketHeader outPktHdr;
    MailHeader outMailHdr;
    const char *data = "ack";

    outPktHdr.to = pktHdr.from;
    DEBUG('r', "SendAck : outPktHdr.to = %d\n", outPktHdr.to);
    outMailHdr.to = mailHdr.from;
    DEBUG('r', "SendAck : outMailHdr.to = %d\n", outMailHdr.to);
    outMailHdr.from = 0; // This instruction is useless since we don't respond to an ack
    outMailHdr.length = strlen(data) + 1;
    outMailHdr.isAck = 1;
    DEBUG('r', "SendAck : outMailHdr.isAck = %d\n", outMailHdr.isAck);
    outMailHdr.ack = mailHdr.ack;
    outMailHdr.last = 1;

    DEBUG('r', "%d tente d'envoyer un ack a %d sur la boite %d avec un ack de %d\n", netAddr, outPktHdr.to, outMailHdr.to, outMailHdr.ack);

    Send(outPktHdr, outMailHdr, data);

    DEBUG('r', "%d Sortie de SendAck\n", netAddr);
}

//----------------------------------------------------------------------
// PostOffice::SendPieces
//  Send a packet with a variable size
//----------------------------------------------------------------------

void 
PostOffice::SendPieces(PacketHeader pktHdr, MailHeader mailHdr, const char *data)
{
    // We copy the data
    char *dataCpy = new char[strlen(data)];
    strcpy(dataCpy, data);

    unsigned int size_data = strlen(data);

    char *buff = new char[MaxMailSize+1];

    int i = 0;
    while(size_data > MaxMailSize) {
        strncpy(buff, dataCpy+i*MaxMailSize, MaxMailSize);
        mailHdr.last = 0;
        mailHdr.length = strlen(buff);
        mailHdr.sequence = i;
        SendReliable(pktHdr, mailHdr, buff);

        size_data -= MaxMailSize;
        i++;
    }

    if (size_data > 0) {
        memset (buff, '\0', (MaxMailSize+1)*sizeof(char));
        strncpy(buff, dataCpy+i*MaxMailSize, size_data);
        mailHdr.last = 1;
        mailHdr.length = strlen(buff);
        mailHdr.sequence = i;
        SendReliable(pktHdr, mailHdr, buff);
    }
}

//----------------------------------------------------------------------
// PostOffice::Receive
// 	Retrieve a message from a specific box if one is available, 
//	otherwise wait for a message to arrive in the box.
//
//	Note that the MailHeader + data looks just like normal payload
//	data to the Network.
//
//
//	"box" -- mailbox ID in which to look for message
//	"pktHdr" -- address to put: source, destination machine ID's
//	"mailHdr" -- address to put: source, destination mailbox ID's
//	"data" -- address to put: payload message data
//----------------------------------------------------------------------

void
PostOffice::Receive(int box, PacketHeader *pktHdr, MailHeader *mailHdr, char* data)
{
    ASSERT((box >= 0) && (box < numBoxes));

    boxes[box].Get(pktHdr, mailHdr, data);
    ASSERT(mailHdr->length <= MaxMailSize);
}

//----------------------------------------------------------------------
// PostOffice::IncomingPacket
// 	Interrupt handler, called when a packet arrives from the network.
//
//	Signal the PostalDelivery routine that it is time to get to work!
//----------------------------------------------------------------------

void
PostOffice::IncomingPacket()
{ 
    messageAvailable->V(); 
}

//----------------------------------------------------------------------
// PostOffice::PacketSent
// 	Interrupt handler, called when the next packet can be put onto the 
//	network.
//
//	The name of this routine is a misnomer; if "reliability < 1",
//	the packet could have been dropped by the network, so it won't get
//	through.
//----------------------------------------------------------------------

void 
PostOffice::PacketSent()
{ 
    messageSent->V();
}

//----------------------------------------------------------------------
// PostOffice::GetNetworkAddress
//  Return the network address of the current machine
//----------------------------------------------------------------------

NetworkAddress
PostOffice::GetNetworkAddress()
{ 
    return netAddr;
}

//----------------------------------------------------------------------
// PostOffice::FindAck
//  Interrupt handler, called when a packet arrives from the network.
//
//  Signal the PostalDelivery routine that it is time to get to work!
//----------------------------------------------------------------------

void
PostOffice::FindAck(MailHeader* mailHdr)
{
    ackLock->Acquire();
    while(ackTable[mailHdr->from][ackNumber[mailHdr->from] % NbAckTable] != 0) {
        ackNumber[mailHdr->from]++;
    }
    mailHdr->ack = ackNumber[mailHdr->from]++;
    ackTable[mailHdr->from][mailHdr->ack % NbAckTable] = 1;

    ackLock->Release();
}