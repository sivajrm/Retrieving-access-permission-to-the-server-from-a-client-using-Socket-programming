/************* UDP SERVER CODE *******************/

//SIVA SIVASUBRAMONIAM JAYARAM 

//INPUT : PACKET WITH SOURCE NUMBER, TECHNOLOGY OF A CUSTOMER
//OUTPUT: PACKET DESCRIBING THE ACCESS PERMISSION TO THE NETWORK OF A CUSTOMER 

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>



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
    int udpSocket, nBytes;
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size, client_addr_size;
    int i;
    unsigned int upcoming_packet_no=1;

   									 /*Create UDP socket*/
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    									/*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

   									 /*Bind socket with address struct*/
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  									  /*Initialize size variable to be used later on*/
    addr_size = sizeof serverStorage;
    									  /*Initialise ACK packet for response to Client*/
    struct Ack_packet serverAckPacket = {0xffff,1,0xfffb,99,10,02,666,0xffff}; //init the ack packet structure

    struct Data_packet dp2;
    int accessCode;
    while(1) {
       									 /*Reset output buffer for console printing*/
        setbuf(stdout, NULL);
        fflush(stdout);

      									  /* Receive any incoming UDP datagram. Address and port of
      									     requesting client will be stored on serverStorage variable */
        nBytes = recvfrom(udpSocket,&dp2,1024,0,(struct sockaddr *)&serverStorage, &addr_size);  //RECV PACKET FROM CLIENT
        printf("\n\nExpected packet number:%d Input packet number:%u\n",upcoming_packet_no,dp2.segment_no);
        if(upcoming_packet_no >= dp2.segment_no) {

            /*Init file pointer with read mode to read the DB contents*/
            FILE *fptr;
            fptr = fopen("/home/siva/Desktop/CN/Verification_DB.txt","r");

            if(fptr == NULL)
            {
                //if fptr is NULL, then terminate the program and return.
                printf("Error in reading the DB at the specified location. Retry with correct location");
                exit(1);
            }
            else {
                printf("\n---INCOMING PACKET FROM CLIENT----\n---START_OF_PACKET-------CLIENT_ID--------ACCESS----------SEGMENT_NO---------LENGTH-----TECHNOLOGY---------SOURCE-------------END_OF_PACKET-----------------------");
                printf("\n-------%x------------------%u-------------%x---------------%u-----------------%u------------%u-----------%u----------------%x---------------------------",
                       dp2.start_of_packet_id,dp2.client_id,dp2.access_permission,dp2.segment_no,dp2.length,dp2.technology,dp2.source_subscriber_number,dp2.end_of_packet_id);

                char line[256],*token;
                int flag=0;

                while (fgets(line, sizeof(line), fptr)) {	/*Read line by line in the text file*/
                    int value=1;

                    printf("\nDATA =%s\n", line);   		//Data in file
                    token = strtok(line," ");		  	/* walk through other tokens of a single customer entry */

                    while( token != NULL ) {
                        					/* for each line compare the source number, if there is a match, go for technology then for paid  status.*/

                        /* Value = 1  -> SOURCE NUMBER
                                 = 2  -> TECHNOLOGY
                       		 = 3  -> PAID    */

                        if(value == 1) {
                            if(dp2.source_subscriber_number==atol(token)) {
                                printf("SOURCE MATCHED -> ");
                                //printf("dp2 source:%u - %ld\t",dp2.source_subscriber_number,atol(token));
                                flag=1;
                            }
                            else {
                                //printf("MATCH NOT FOUND"); //go for other line, since there is NO MATCH for source number.
                                flag=-999;
                                break;
                            }
                        }
                        else if(value == 2) {
                            //printf("Tech:");
                            if(dp2.technology == atol(token)) {
                                printf("TECHNOLOGY MATCHED -> ");
                                flag=1;
                            }
                            else {
                                //printf("dp2 tech:%u - %ld\t",dp2.technology,atol(token));
                                printf("TECHNOLOGY NOT MATCHED");
                                flag=-1;
                                break;
                            }
                        }
                        else if(value == 3) {
                            if(atol(token)) {
                                printf("PAID ");
                                flag=999;
                                break;
                            }
                            else {
                                printf("NOT PAID");
                                flag=-99;
                                break;
                            }
                        }

                        token = strtok(NULL," "); 		//process other entries of a customer
                        value+=1;
                    }

                    if((flag==-1)||(flag == 999)||(flag == -99)) {
                        					//check the flag and assign ACCESS-CODE appropriately
                        if(flag == -1) {
                            accessCode = 0xfffa;
                            //printf("\nNOT EXIST");
                        }
                        else if(flag == 999) {
                            accessCode = 0xfffb;
                            //printf("\nPAID");
                        }
                        else if(flag == -99) {
                            accessCode = 0xfff9;
                            //printf("\nNOT PAID");
                        }
                        break;
                    }
                }
                if(flag == -999){  				//still if flag is -999 no matching entries found, hence  NOT EXIST
                    accessCode = 0xfffa;
		    printf("SOURCE NOT MATCHED");		//NOT EXIST
		}	        				
                }

            fclose(fptr);					//close the file pointer

            printf("\n\nSend ACK ");
           							 //send ACK for that packet with the current packet number, its accesscode, technology and source.
            serverAckPacket.recvd_segment_no=dp2.segment_no;
            serverAckPacket.status=accessCode;
            serverAckPacket.technology=dp2.technology;
            serverAckPacket.source_subscriber_number=dp2.source_subscriber_number;
            printf("\n---START_OF_PACKET-------CLIENT_ID--------STATUS----------RCVD_SEGMENT_NO--------LENGTH------TECHNOLOGY----------SOURCE---------------END_OF_PACKET--------------");
            printf("\n-------%x------------------%u-------------%x------------------%u------------------%u------------%u-------------%u------------------%x------------------",
                   serverAckPacket.start_of_packet_id,serverAckPacket.client_id,serverAckPacket.status,serverAckPacket.recvd_segment_no,serverAckPacket.length,serverAckPacket.technology,
                   serverAckPacket.source_subscriber_number,serverAckPacket.end_of_packet_id);
            sendto(udpSocket,&serverAckPacket,nBytes,0,(struct sockaddr *)&serverStorage,addr_size); //SEND ACK TO CLIENT
            upcoming_packet_no+=1;

        }

    }

    return 0;
}
