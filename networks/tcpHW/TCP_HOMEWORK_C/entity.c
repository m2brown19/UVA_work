/******************************************************************************/
/*                                                                            */
/* ENTITY IMPLEMENTATIONS                                                     */
/*                                                                            */
/******************************************************************************/
#define WINDOW_SIZE 8;
// Student names: Michael Brown
// Student computing IDs: mjb4us
//
//
// This file contains the actual code for the functions that will implement the
// reliable transport protocols enabling entity "A" to reliably send information
// to entity "B".
//
// This is where you should write your code, and you should submit a modified
// version of this file.
//
// Notes:
// - One way network delay averages five time units (longer if there are other
//   messages in the channel for GBN), but can be larger.
// - Packets can be corrupted (either the header or the data portion) or lost,
//   according to user-defined probabilities entered as command line arguments.
// - Packets will be delivered in the order in which they were sent (although
//   some can be lost).
// - You may have global state in this file, BUT THAT GLOBAL STATE MUST NOT BE
//   SHARED BETWEEN THE TWO ENTITIES' FUNCTIONS. "A" and "B" are simulating two
//   entities connected by a network, and as such they cannot access each
//   other's variables and global state. Entity "A" can access its own state,
//   and entity "B" can access its own state, but anything shared between the
//   two must be passed in a `pkt` across the simulated network. Violating this
//   requirement will result in a very low score for this project (or a 0).
//
// To run this project you should be able to compile it with something like:
//
//     $ gcc entity.c simulator.c -o myproject
//
// and then run it like:
//
//     $ ./myproject 0.0 0.0 10 500 3 test1.txt
//
// Of course, that will cause the channel to be perfect, so you should test
// with a less ideal channel, and you should vary the random seed. However, for
// testing it can be helpful to keep the seed constant.
//
// The simulator will write the received data on entity "B" to a file called
// `output.dat`.

#include <stdio.h>
#include <stdbool.h>
#include "simulator.h"

//HELPER FN FOR CHECKSUM
// int doChecksum(int seqnum, int acknum, int length, char payload[20]) {
//     int checksum = seqnum + acknum + length; 
//     for ( int i = 0; i < length; i++) {
//         checksum += payload[i]; //add the chars to it
//     }
//     return checksum;
// }


/**** A ENTITY ****/

int seqnum_A;
int acknum_A;
int pktCountA;
int curPktIndex;
//int window; //added. use to keep track of base of window. 
struct pkt curPacket;
struct pkt pktArray[634]; //SIZE MAY NEED TO BE HANDLED DIFFERENTLY...
//bool received_good_ack = true; //use this to stop A from sending until it receives a good ACK

void A_init() {
    seqnum_A = 0;
    acknum_A = 0;
    pktCountA = 0;
    curPktIndex = 0; //keeps track of current packet index
    //window = 0;
}

void A_output(struct msg message) {
    //printf("A OUTOPUT CALLED AGAIN\r\n");
    
    //turn msg into a packet
    struct pkt mypkt;
    mypkt.length = message.length;

    //until the good ACK is received, stay on the current SEQ NUM and dont send the next part of msg. 
    // while (received_good_ack == false) {
    //     ; //infin loop till it 
    // }
    
    mypkt.seqnum = seqnum_A;
    mypkt.acknum = acknum_A;

    //printf("MESSAGE IS: %s\r\n", message.data);

    
    for (int i = 0; i < message.length; i++) {
        
        mypkt.payload[i] = message.data[i];
        curPacket.payload[i] = message.data[i];
    }

    //Calculate checksum
    //treat char as 8 bit int. add to each char
    //then add seq num and ack num. TODO
    mypkt.checksum = seqnum_A + acknum_A + mypkt.length; 
    for ( int i = 0; i < mypkt.length; i++) {
        mypkt.checksum += mypkt.payload[i]; //add the chars to it
    }
    //mypkt.checksum = doChecksum(seqnum_A, acknum_A, mypkt.length, mypkt.payload);
    //printf("SENDER CHECKSUM IS: %d\r\n", mypkt.checksum);
    //printf("Other checksum %d\r\n", message.checksum);
    //one's complement
    //mypkt.checksum = mypkt.checksum;
    //go back n change
    seqnum_A++;
    acknum_A++;


    //think seqnum and acknum should be same for alt bit.... might need to change later

    //Save info to curPacket in case need to retransmit
    // curPacket.length = mypkt.length;
    // //payload set above
    // curPacket.seqnum = mypkt.seqnum;
    // curPacket.acknum = mypkt.acknum;
    // curPacket.checksum = mypkt.checksum;

    
     
    pktArray[pktCountA] = mypkt;

    pktCountA++;

    //only send it to B from here if no packet is in transit. 
    //before incrementing pkt count, cur pkt index caught up to it, meaning it got a GOOD ACK
    //cur pkt index only increments when it gets a GOOD ACK. 
    //if dont send here, then once i receive a GOOD ACK for packet in transit, then i will send next from
    //the input fn to catch up. should handle if packets are waiting 
    // if (pktCountA - 1 == curPktIndex) {
    //     tolayer3_A(mypkt);
    //     starttimer_A(200.0);
    // }
    //go back n change. if the index of inserted new packet is less or equal to the end of window index
    //send it right away. Otherwise, it will be sent in input fn when the window shifts to get to it
    if (pktCountA - 1 <= curPktIndex + 7) {
        tolayer3_A(mypkt);
        starttimer_A(200.0);
    }

    
    
    //SEND AND START TIMER
    // if (pktCountA == 0){
    //     pktCountA++;
    //     tolayer3_A(mypkt); //pass packet now from layer 4 (its a segment now) to layer 3 across network to B
    //     //ONLY START TIMER HERE ON FIRST PACKET... for others, start it in input fn after recv
    //     // a ACK or in timer interrupt FN
    //     //EDIT
    //     //starttimer_A(200.0);
    // } else {
    //     pktCountA++;
    //     // if (pktCountA > 9) {
    //     //     pktCountA = 9; // Dont let it go over max array index
    //     // }
    // }
    

    //after sending, change the seq and ack nums. TODO CHECK -- sending prolly should not set the correct ACK NUM...
    // if (seqnum_A == 0) {
    //     seqnum_A = 1;
    //     acknum_A = 1;
    // } else {
    //     seqnum_A = 0;
    //     acknum_A = 0;
    // }
    

    //received_good_ack = false; //after sending, this is false until it receives a good one. 
    //printf("finished A Output");
}

void A_input(struct pkt packet) {
    //printf("In input fn\r\n");
    //Check checksum
    int calcCheckSumAck = packet.seqnum + packet.acknum + packet.length;
    for ( int i = 0; i < packet.length && i < 20; i++) {
        calcCheckSumAck += packet.payload[i]; //add the chars to it
    }
    //is it payload access if length is off?

    //packets in an array
    //when receive it, even index packets should have seqnum zero. odd should have seqnum = 1
    //if (curPktIndex % 2 == packet.acknum && calcCheckSumAck == packet.checksum) {
    //printf("CURPKTINDEX = %d\r\n", curPktIndex);
    //printf("Packet ack num = %d\r\n", packet.acknum);
    //calcCheckSumAck
    if (curPktIndex <= packet.acknum && calcCheckSumAck == packet.checksum) {
        //printf("A_INPUT FN: PACKET IS GOOD, same seqnum for A as acknum received. \r\n");
        //stop timer here because got good packet. If bad, timer will fire eventually. 
        stoptimer_A();
        //changed condition and curpktindex to adjust based off of the acknum recevied.
        //if ack packet is lost, but the ack num for a packet received is larger, then i know 
        //that B received the earlier msg for fwhich i missed the ack. 
        //printf("GOOD PACKET ACK NUM IS: %d\r\n", packet.acknum);

        curPktIndex = packet.acknum + 1; //update it, this packet was received GOOD. 
        //received_good_ack = true;
        //printf("NEW CUR INDEX IS IS: %d\r\n", curPktIndex);
        

        //DETERMINE IF NEED TO SEND FROM HERE ---- if there is a line of packets waiting to be sent
        //after incrementing packet index, if it is still less than packet count, it is behind and 
        //there is a line
        // if (curPktIndex < pktCountA) {
        //     //send next packet waiting in line (what current points to)
        //     tolayer3_A(pktArray[curPktIndex]);
        //     starttimer_A(200.0);
        // }
        //go back n change. determine if there is another packet to send after shifting window. 
        if (curPktIndex < pktCountA) {
            starttimer_A(200.0); //timer needs to keep going until it is done
        }
        if (curPktIndex + 7 <= pktCountA) {
            tolayer3_A(pktArray[curPktIndex + 7]);
            
        }
        
        
        //EDIT
        //stoptimer_A();
        //if (pktArray[curPktIndex] != NULL) { 
        //WILL NEED BACK LATER 
        //tolayer3_A(pktArray[curPktIndex]); //pass packet now from layer 4 (its a segment now) to layer 3 across network to B
        //}
        //starttimer_A(200.0);
        //Packet is good. it was ACKed with the same num as seq num...

        //Change seq num and ack num and send next packet
        // if (seqnum_A == 0) {
        //     seqnum_A = 1;
        //     acknum_A = 1;
        // } else {
        //     seqnum_A = 0;
        //     acknum_A = 0;
        // }
    } else {
        //printf("A_INPUT FN: ACK PACKET DONT HAVE SAME SEQ NUM \r\n");
        //I think B should just not send anything if bad packet
        //Retransmit the same packet if not good
        //EDIT
        //stoptimer_A();
        //tolayer3_A(pktArray[curPktIndex]); //this is problematic for huge files....
        //starttimer_A(200.0);
    }


}

void A_timerinterrupt() {
    //timer interrupts so... resend current packet...
    //stoptimer_A();
    //printf("TIMER INT FN--------------\r\n");
    // printf("PACKET COUNT A= %d\r\n", pktCountA);
    // printf("Cur pkt index = %d\r\n", curPktIndex);
    
    // tolayer3_A(pktArray[curPktIndex]);
    // starttimer_A(200.0);

    //Resend entire window (that is available)
    for (int i = 0; (curPktIndex + i) < pktCountA && (i < 8); i++) {
        //if (i < 8) {
        //printf("Cur pkt index + i = %d\r\n", curPktIndex + i);
        tolayer3_A(pktArray[curPktIndex + i]);
        //}
    
    }
    //printf("Done timer int, start it up again\r\n");
    starttimer_A(200.0);
}


/**** B ENTITY ****/
int seqnum_B;
int acknum_B;
int last_seq_num = -1;
int countPacketsB = 0; //use to help know if first packet or not for last seq num comparison.
int cumulativeACK = -1;
//struct pkt bpktsRecvd[634]; //store packets received in here. 

void B_init() {
}

void B_input(struct pkt packet) {
    //printf("IN B INPUT");
    //when receiving packet, check the checksum,
    int calcCheckSum = packet.seqnum + packet.acknum + packet.length;
    for ( int i = 0; i < packet.length && i < 20; i++) {
        calcCheckSum += packet.payload[i]; //add the chars to it
    }
    //int calcCheckSum = doChecksum( packet.seqnum, packet.acknum, packet.length, packet.payload[20]);
    //one's complement
    //calcCheckSum = calcCheckSum;
    //printf("b CHECKSUM IS: %d\r\n", calcCheckSum);
    //printf("b CHECKSUM IS: %d\r\n", packet.checksum);


    if (packet.checksum == calcCheckSum) {
        

        //if checksum is good, send ACK matching sequence num back to A
        struct pkt ackPack;
        //ackPack.acknum = packet.seqnum;
        ackPack.length = 0;
        //ackPack.seqnum = packet.seqnum;

        

        //tolayer3_B(ackPack);
        //go back n change. do not make ack packet till i know it is good with cumulative ack n


        //and send packet up from A to layer 5 in B if not duplicate
        
        //was comparing last seq num to packet seqnum, but comparing diff way now. 
        //countPacketsB % 2
        //if (last_seq_num == packet.seqnum && countPacketsB != 0) {
        if (packet.seqnum != cumulativeACK + 1) {
            //Duplicates, do not send to layer 5 B.
            //printf("last seq num = %d\r\n", last_seq_num);
            //printf("Duplicate Packet!\r\n");
            //printf("not the correct packet\r\n");
            //go back n change
            //re-send an ack to A. A needs to check if this is the correct ack packet now
            ackPack.seqnum = cumulativeACK;
            ackPack.acknum = cumulativeACK;
            //set checksum of this ACK Packet
            ackPack.checksum = ackPack.seqnum + ackPack.acknum + ackPack.length;
            for ( int i = 0; i < ackPack.length; i++) {
                ackPack.checksum += ackPack.payload[i]; //add the chars to it
            }
            tolayer3_B(ackPack);

            
        } else {
            //printf("GOOD PACKET RECVD, sending good ACK\r\n");
            cumulativeACK++; //got a Good packet, increment cumulative ack. 
            //printf("CUMULATIVE ACK = %d\r\n", cumulativeACK);
            ackPack.seqnum = cumulativeACK;
            ackPack.acknum = cumulativeACK;
            //set checksum of this ACK Packet
            ackPack.checksum = ackPack.seqnum + ackPack.acknum + ackPack.length;
            for ( int i = 0; i < ackPack.length; i++) {
                ackPack.checksum  += ackPack.payload[i]; //add the chars to it
            }
            tolayer3_B(ackPack);
            //Store packet in packet array and increment count of packets
            //bpktsRecvd[countPacketsB];
            countPacketsB++;

            //last_seq_num = packet.seqnum; //update last seq num to current. 
            
            struct msg newMsgToB;
            newMsgToB.length = packet.length;
            for (int i = 0; i < newMsgToB.length; i++) {
                newMsgToB.data[i] = packet.payload[i];
            }
            tolayer5_B(newMsgToB);

            

        }
        
        
        
    }
}

void B_timerinterrupt() {
}
