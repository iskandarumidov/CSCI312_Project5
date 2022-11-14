#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//sockets
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


#include "Philosopher.h" //provides philosopher object

#define INTERVAL_MAX	7//Maximum time interval for random number generation
#define INTERVAL_MIN	3//Minimum time interval for random number generation
#define WRITE_BUF_SZIE	20 //Buffer size
#define RUN_COUNT	15 ///this program will eat and think this many times

/*
 * This function simulates the thinking process for a philospoher with random think time
 * @param _phil Philosoher object which containg philosopher id which will think
 * @return none
 */
void philosopherIsThinking(Philosopher _phil){
	//seed rand
	//srand(time(0));
	int _randomInterval = (random() % (INTERVAL_MAX - INTERVAL_MIN + 1)) + INTERVAL_MIN; //get random interval for sleep
	printf("CentralizedClient: Philosopher(%d) has started thinking. Time: %i\n",_phil.getId(),(int)time(NULL));
	sleep(_randomInterval); //sleep
	printf("CentralizedClient: Philosopher(%d) has completed thinking. Time: %i\n",_phil.getId(),(int)time(NULL));
} 

/*
 * This function simulates the eating process of a philospher with random eat time
 * @param _phil philospoher object that container philosopher id tha is used to think
 * @return none
 */
void philosopherIsEating(Philosopher  _phil){
	//seed rand
	//srand(time(0));
	//
	
	//reuest server for chosticks 
	char _sendBuf[WRITE_BUF_SZIE];
	sprintf(_sendBuf,"%i_E!",_phil.getId());
	_sendBuf[WRITE_BUF_SZIE] = '\0';
	int _err = send(_phil.getFd(),_sendBuf,WRITE_BUF_SZIE,0); //send reuest to server
	if(_err == -1){
		perror("CentralizedClient: Error While Sending Id to Server");
		exit(3);
	}
	
	
	//waitfor server's response to request
	char _recvBuf[WRITE_BUF_SZIE];
	_err = recv(_phil.getFd(),_recvBuf,strlen(_recvBuf),0); //recevive the request
	if(_err == -1){
		perror("CentralizedClient: Error While Receving from Server");
		exit(4);
	}
	else{
		
		//start eating
		printf("CentralizedClient: Philosopher(%d) has started eating. Time: %i \n",_phil.getId(),(int)time(NULL));
		int _randomInterval = (random() % (INTERVAL_MAX - INTERVAL_MIN + 1)) + INTERVAL_MIN;
		sleep(_randomInterval);
		printf("CentralizedClient: Philosopher(%d) has finished eating. Time: %i \n",_phil.getId(),(int)time(NULL));
		
		//Notifying server that philosopher has completed eating
		sprintf(_sendBuf,"%i_F!",_phil.getId());
		_err = send(_phil.getFd(),_sendBuf,strlen(_sendBuf),0);
		if(_err == -1){
			perror("CentralizedClient: Error While Sending Id to Server");
			exit(5);
		}
	}	
}

/*
 * this function create a client socket descriptor and return the descriptor
 * return client socket file descritor
 */
int createClientSocketDescriptorAndConnectToServer(){
	//create client socket
	int _clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(_clientSocket == -1){
		perror("CentralizedClient: Creation of Clinet Socket Descriptor failed");
		exit(1);
	}
	
	//specifiy and address for socket
	struct sockaddr_in _serverAddress;
	_serverAddress.sin_family = AF_INET; //IPV4 Family Protocol
	_serverAddress.sin_port = htons(9112);
	_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	//connect to remote server
	int _err = connect(_clientSocket, (struct sockaddr*)&_serverAddress,sizeof(_serverAddress));
	if(_err == -1){
		perror("CentralizedClient: Cannot Connect to Server");
		exit(2);
	}
	
	return _clientSocket; //return socket descriptor
}



/*
 * This is tarting point for this program
 * @this main routine accepts client id as argv[1]
 */
int main(int _argc, char * _argv[]){

	int _clientSocketFd = createClientSocketDescriptorAndConnectToServer();//create client socket descriptor

	Philosopher _phil(atoi(_argv[1]),_clientSocketFd);//get client id(_argv[1]) and create philospopher object	
	
	srand(time(0)); //seed random

	int _count = 0; 
	for(;;){ //start continuos client simulation loop
		
		philosopherIsEating(_phil); //eat
		philosopherIsThinking(_phil); //think
		_count = _count + 1; //incremet activity counts
		if(_count > RUN_COUNT){ //if the activinty coutnreaches a constant time then exit
			printf("CentralizedClient: About to Exit. This Client ran % times\n",RUN_COUNT);
			exit(1);
		}
	}
	
	return 0;
}
