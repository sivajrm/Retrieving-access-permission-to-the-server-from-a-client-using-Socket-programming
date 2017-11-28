/************* UDP CLIENT CODE *******************/

//SIVA SIVASUBRAMONIAM JAYARAM 

//INPUT : PACKET WITH SOURCE NUMBER, TECHNOLOGY OF A CUSTOMER
//OUTPUT: PACKET DESCRIBING THE ACCESS PERMISSION TO THE NETWORK OF A CUSTOMER 

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

struct Data_packet {
    uint16_t start_of_packet_id;   	//2 bytes
    uint8_t client_id; 			//1 byte
    uint16_t access_permission;    	//2 bytes
    uint8_t segment_no; 		//1 byte
    uint8_t length;			//1 byte
    uint8_t technology;			//1 byte
    uint32_t source_subscriber_number;	//4 bytes
    uint16_t end_of_packet_id;		//1 byte
};

struct Ack_packet {
    uint16_t start_of_packet_id;   	//2 bytes
    uint8_t client_id; 			//1 byte
    uint16_t status;    		//2 bytes
    uint8_t recvd_segment_no; 		//1 byte
    uint8_t length;			//1 byte
    uint8_t technology;			//1 byte
    uint32_t source_subscriber_number;	//4 bytes
    uint16_t end_of_packet_id;		//1 byte
};


int main() {
    int clientSocket, portNum, nBytes;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    int packetToSend;
    int retryCounter;
    												/*Create UDP socket*/
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    												/*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    												/*Initialize size variable to be used later on*/
    addr_size = sizeof serverAddr;
    struct Ack_packet ackPack;

    ackPack.recvd_segment_no=0;
    packetToSend=1;
    int alterPacket=0;
    retryCounter=0;
    char *errString;
    while(1) {
        setbuf(stdout, NULL);
        fflush(stdout);
        struct Data_packet dp1 = {0xffff,1,0xfff8,packetToSend,10,02,4089999999,0xffff}; 	//init the outgoing DATA packet.

        if(retryCounter==0) {

            printf("\n\nAlter source and Technology 0 or 1 ?");
            scanf("%d",&alterPacket);

            if(alterPacket) {
                printf("\nEnter source number:");
                scanf("%" SCNu32,&dp1.source_subscriber_number);

                printf("Enter the Technology:");
                scanf("%" SCNu8,&dp1.technology);
            }
        }
        printf("\n---OUTGOING PACKET----\n---START_OF_PACKET-------CLIENT_ID--------ACCESS----------SEGMENT_NO------------LENGTH-------------TECHNOLOGY--------------SOURCE----------------END_OF_PACKET---------");
        printf("\n-------%x-----------------%u--------------%x---------------%u-------------------%u---------------------%u----------------%u--------------------%x------------",
               dp1.start_of_packet_id,dp1.client_id,dp1.access_permission,dp1.segment_no,dp1.length,dp1.technology,dp1.source_subscriber_number,dp1.end_of_packet_id);

        nBytes = sizeof(dp1) + 1;
        if(alterPacket!=-1) {
           											 /*Send message to server*/
            sendto(clientSocket,&dp1,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);	//OUTGOING PACKET FROM CLIENT
            time_t t1 = time(0); 								//OUT TIME FROM CLIENT


            struct timeval timeout= {3,0}; 							//set timeout for 3 seconds

            setsockopt(clientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));	/* set receive UDP message timeout */

            											/*Receive message from server*/
            nBytes = recvfrom(clientSocket,&ackPack,1024,0,NULL, NULL);				//RECV PACKET FROM SERVER
            time_t t2 = time(0);								//IN TIME OF THE PACKET FROM SERVER
            if((nBytes>0)&&(difftime(t2, t1)<=3)&&(retryCounter<=3)) {			        //CHECK THE TIME_LIMIT
                retryCounter=0;
                printf("\n\nReceived...\nIn:%s - Out:%s",ctime(&t1),ctime(&t2));
                if(ackPack.status == 0xfffa)
                    errString="Not Exist";
                else if(ackPack.status == 0xfffb)
                    errString="Paid";
                else if(ackPack.status == 0xfff9)
                    errString="Not Paid";

                printf("\n-----ACK: %s---\n\n-------START_OF_PACKET-------CLIENT_ID--------STATUS--------RCVD_SEGMENT_NO--------LENGTH--------TECHNOLOGY---------SOURCE---------END_OF_PACKET------",errString);
                printf("\n-----------%x------------------%u-------------%x----------------%u-------------------%u--------------%u-----------%u------------%x----------",
                       ackPack.start_of_packet_id,ackPack.client_id,ackPack.status,ackPack.recvd_segment_no,ackPack.length,ackPack.technology,ackPack.source_subscriber_number,ackPack.end_of_packet_id);
                packetToSend=ackPack.recvd_segment_no+1;
            }
            else {
               											 //if timeout limit exceeded, then retry for a max of 3 times.
                if(retryCounter == 3) {
                    printf("\n/*******************  SERVER DOES NOT RESPOND  *************************/\n");
                    break;
                }
                int second;
                for(second=1; second<=3; second++) {
                    sleep(1);
                    printf("\nWaiting %d second",second);
                }

                printf("\nNot received retrying:%d",++retryCounter);
            }

        }//case
    }//while-end

    return 0;
}
