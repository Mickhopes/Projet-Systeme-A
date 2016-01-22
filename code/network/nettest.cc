// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads 
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"

// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our 
//	    original message

void
MailTest(int farAddr)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    const char *data = "Hello there!";
    //const char *data = "012345678910111213141516171819202122232425262728293031323334353637383940";
    char buffer[MaxMailSize];
    int i = 0;
    while(i < 10) {
        // construct packet, mail header for original message
        // To: destination machine, mailbox 0
        // From: our machine, reply to: mailbox 1
        outPktHdr.to = farAddr;     
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(data) + 1;
        
        // Send the first message
        postOffice->SendReliable(outPktHdr, outMailHdr, data); 

        // Wait for the first message from the other machine
        postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
        printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.to);
        fflush(stdout);
        i++;
    }

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    /*outPktHdr.to = farAddr;     
    outMailHdr.to = 0;
    outMailHdr.from = 1;
    outMailHdr.length = strlen(data) + 1;
    
    // Send the first message
    postOffice->SendPieces(outPktHdr, outMailHdr, data); 

    memset(buffer, '\0', MaxMailSize*sizeof(char));
    // Wait for the first message from the other machine
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.to);
    fflush(stdout);

    memset(buffer, '\0', MaxMailSize*sizeof(char));
    // Wait for the first message from the other machine
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.to);
    fflush(stdout);

    memset(buffer, '\0', MaxMailSize*sizeof(char));
    // Wait for the first message from the other machine
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.to);
    fflush(stdout);*/

    currentThread->Sleep(10000000);

    // Then we're done!
    interrupt->Halt();
}

void
RingTest(int numMachines)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    const char *data = "Token!";
    char buffer[MaxMailSize];

    if (postOffice->GetNetworkAddress() == 0) {
        // The first machine will send it's token to the second one
        outPktHdr.to = 1;
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(data) + 1;

        // We send it
        postOffice->SendReliable(outPktHdr, outMailHdr, data);

        // Wait for last machine to send the token
        postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
        printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.to);
        fflush(stdout);
    } else {
        // Wait for previous machine to send the token
        postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
        printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.to);
        fflush(stdout);

        // We keep the token 2 seconds
        Delay(2);

        outPktHdr.to = postOffice->GetNetworkAddress() == numMachines-1 ? 0 : postOffice->GetNetworkAddress()+1;
        DEBUG('r', "Machine %d doit envoyer a %d\n", postOffice->GetNetworkAddress(), outPktHdr.to);
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(buffer) + 1;

        // We send it
        postOffice->SendReliable(outPktHdr, outMailHdr, buffer);
    }

    currentThread->Sleep(100000000);

    // Then we're done!
    interrupt->Halt();
}