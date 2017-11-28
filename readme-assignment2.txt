//SIVA SIVASUBRAMONIAM JAYARAM 
//CLIENT SERVER UDP

//INPUT : PACKET WITH SOURCE NUMBER, TECHNOLOGY OF A CUSTOMER
//OUTPUT: PACKET DESCRIBING THE ACCESS PERMISSION TO THE NETWORK OF A CUSTOMER 


Program execution steps:

1. Open a new terminal
2. Execute "gcc server2.c" for establishing connection on the directory location which has this file server2.c and output through typing the command "./a.out"
3. Open another new terminal
4. Execute "gcc client2.c" for establishing connection on the directory location which has this file client2.c and output through typing the command "./a.out"
5. Database .txt file has to be updated in the server code "/home/siva/Desktop/CN/Verification_DB.txt"
6. In "Client terminal" -> input the choice of altering the source number and technology through entering either 1 or 0 | 1 -> To alter | 0 -> Default packet
7. Get the output from the server


ASSUMPTIONS
-> Client id is 1 for all executions.
-> Source subscriber number is 10 numbers, hence the length of the payload is always 1O.
-> Make sure the verification-db points to the correct location in the code. 
-> ACCESS CODE 0xfff8 client requesting access to the network
-> CODES used
	0xfff	=> 	Start/End of packet
-> REJECT subcodes used
	0xfff4	=>	Out of sequence
	0xfff5	=>	Length mismatch
	0xfff6	=>	End of packet missing
	0xfff7	=>	Duplicate packets
-> STATUS CODE from server with explanation
	 0xfffa "Not Exist" 		        => Technology mismatch || Customer record not found in the database
     0xfffb "Paid with Access"	    	=> Record found with payment status satisfied hence allowed full access to the network
	 0xfff9	"Not Paid"                  => Record found with payment status not satisfied
