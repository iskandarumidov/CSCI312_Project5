//Standarad libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vector>
#include <queue>
#include <string.h>
#include <string>
#include <iostream>
#include <stdbool.h>

//linux libraries
#include <unistd.h>
#include <errno.h>
#include <time.h>

//socket libraries
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

//pthreads
#include <pthread.h>

//custom library
#include "Philosopher.h" //provides philosopher object

using namespace std;

#define WRITE_BUF_SZIE	20 //buffer size for socket communication
#define NUMBER_OF_PHILOSOPHERS 5 //totla numberof clients/philosophers
#define GENERAL_SLEEP_TIME	2 //sleep duration for threads
#define GENERAL_USLEEP_TIME 500 //other sleep duration for threads


//Mutex for locking Queues
pthread_mutex_t	Mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex5 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex6 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex7 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex8 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex9 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex10 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex11 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mutex12 = PTHREAD_MUTEX_INITIALIZER;


//stuck to hold client socket descriptor for thread safety
typedef struct {
	int _fd;
} ClientFd;


//vector to indicate a chopstick's avialabilty
//vector<boo> _chopstickCollection;
bool _chopstickCollection[NUMBER_OF_PHILOSOPHERS]; 

//Queue to hold clients/philosophers who are requesting chopsticks to eat
queue<Philosopher> PendingChopStickRequests;

//Queue to hold clients/philosophers who are releaseing chopsticks after done eating
queue<Philosopher> PendingChopStickReleases;


/*
 * This function is used to lock stdout stream so that only onethread can acess it at a time
 * @param _dis  char buf/char * to be displayed
 * @return none
 */
void displayString(char * _dis){
	pthread_mutex_lock(&Mutex11); //lock mutex
	cout<<_dis<<endl; //print input string
	pthread_mutex_unlock(&Mutex11); //unlock mutex
}


///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^RELEASE QUEUE ACESSORS WITH MUTEXES FOR THREAD SAFETY ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^///
/*
 * This function is used to get the top philosopher item ChopStick Release Queue 
 * @return _phil philosopher object that was on the top of the queue
 */
Philosopher getFrontChopStickReleasesQueue(){
	Philosopher _phil; //create Philosopher object
	pthread_mutex_lock(&Mutex10);	//lock mutex
	_phil =  PendingChopStickReleases.front(); //get the philosopher item in top of the queue
	pthread_mutex_unlock(&Mutex10); //unlock mutex
	return _phil; //return the top philosopher item
}

/*
 * This function is used to dequeue a philospher object from the ChopStick Release Queue
 * @return none
 */
void dequeuePhilosopherInChopStickReleasesQueue(){	
	pthread_mutex_lock(&Mutex8); //lock mutex
	PendingChopStickReleases.pop(); //dequeue phil object 
	pthread_mutex_unlock(&Mutex8); //unlock item
}

/*
 * This function is used to enqueue a philospher object from the ChopStick Release Queue
 * @param _phil Philosopher object to be enqueued
 * @return none
 */
void enqueuePhilosopherInChopStickReleasesQueue(Philosopher _phil){
	pthread_mutex_lock(&Mutex6); //lock  mutex
	PendingChopStickReleases.push(_phil); //enqueue the philosopher item
	pthread_mutex_unlock(&Mutex6); //unlock mutex
}

/*
 * This function is used to get size of  the ChopStick Release Queue
 * @return size of the queue
 */
int getLengthChopStickReleasesQueue(){
	int _len = 0;
	pthread_mutex_lock(&Mutex4); //lock mutex
	_len = PendingChopStickReleases.size(); //get size
	pthread_mutex_unlock(&Mutex4); //unlock mutex
	return _len; //return size
}


/*
 * This function is used to check if the ChopStick Release Queue is empty or not
 * @return queue's empty status
 */
bool isChopstickReleasesQueueEmpty(){
	bool _isEmpty;
	pthread_mutex_lock(&Mutex2); //lock mutex
	_isEmpty = PendingChopStickReleases.empty(); //check if the queue is empty
	pthread_mutex_unlock(&Mutex2); //unlock mutex
	return _isEmpty; //return empty status
}
///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//////////////////







///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^REQUEST QUEUE ACESSORS WITH MUTEXES FOR THREAD SAFETY ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^////

/*
 * This function is used to get the top philosopher item ChopStick REQUEST Queue 
 * @return _phil philosopher object that was on the top of the queue
 */
Philosopher getFrontChopStickRequestQueue(){
	Philosopher _phil; //create Philosopher object
	pthread_mutex_lock(&Mutex9);//lock mutex
	_phil =  PendingChopStickRequests.front();//get the philosopher item in top of the queue
	pthread_mutex_unlock(&Mutex9);//unlock mutex
	return _phil; //return the top philosopher item
}



/*
 * This function is used to dequeue a philospher object from the ChopStick Request Queue
 * @return none
 */
void dequeuePhilosopherInChopStickRequestsQueue(){
	pthread_mutex_lock(&Mutex7); //lock mutex
	PendingChopStickRequests.pop(); //dequeue phil object 
	pthread_mutex_unlock(&Mutex7);//unlock mutex
}


/*
 * This function is used to enqueue a philospher object from the ChopStick request Queue
 * @param _phil Philosopher object to be enqueued
 * @return none
 */
void enqueuePhilosopherInChopStickRequestsQueue(Philosopher _phil){
	pthread_mutex_lock(&Mutex5);  //lock mutex
	PendingChopStickRequests.push(_phil); //enqueue the philosopher item
	pthread_mutex_unlock(&Mutex5); //unlock mutex
}


/*
 * This function is used to get size of  the ChopStick REQUEST Queue
 * @return size of the queue
 */
int getLengthChopStickRequestsQueue(){
	int _len = 0;
	pthread_mutex_lock(&Mutex3);  //lock mutex
	_len = PendingChopStickRequests.size(); //get size
	pthread_mutex_unlock(&Mutex3);  //unlock mutex
	return _len; //return size
}


/*
 * This function is used to check if the ChopStick REQUEST Queue is empty or not
 * @return queue's empty status
 */
bool isChopStickRequestsQueueEmpty(){
	bool _isEmpty;
	pthread_mutex_lock(&Mutex1);  //lock mutex
	_isEmpty = PendingChopStickRequests.empty(); //check if the queue is empty
	pthread_mutex_unlock(&Mutex1);  //unlock mutex
	return _isEmpty; //return empty status
}

///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//////////////////



/*
 * This function handles client/philosopers connections,
 *  it commicates with clients using read(recv)/write(send) commands
 *  it parses data packets from clients, data apacekts contain client ide and reuest type
 *  if the request type is "E" then the client is enqueue in Request queue list else 
 *  if the reuest type is "F" then the client is enqueue in Release queue
 *  @param _clientSocketFd client socket descriptor
 *  @return none
 */
void handleClientConnection(int _clientSocketFd){
	
	for(;;){ //infine loop for continus connection
		
		//Read Client's/Philosopher's Request or Comannd
		char _recvBuf[WRITE_BUF_SZIE];
		int _err = recv(_clientSocketFd,_recvBuf,WRITE_BUF_SZIE,0); //read recived data packet
		_recvBuf[WRITE_BUF_SZIE] = '\0';
		if(_err == -1){
			perror("CentralizedServer: Error In Recv");
			exit(3);
		}
	
		//Parse Id and message from the Message sent by client
		int _uPos  = ((string)_recvBuf).find("_");
		int _exPos = ((string)_recvBuf).find("!");
		string _readPtr = ((string)_recvBuf).substr(0,_uPos);
		int _philosopherId = stoi(_readPtr);
		_readPtr = ((string)_recvBuf).substr(_uPos+1,(_exPos-_uPos-1));
		
		
		//create philosophers and set their chopstick index
		Philosopher _philTemp(_philosopherId,_clientSocketFd,NUMBER_OF_PHILOSOPHERS); 

		//Peform Queue Operations based on Client's Message
		if(_readPtr == "E"){ //if the client/philosopher requeted to eat
			enqueuePhilosopherInChopStickRequestsQueue(_philTemp); //push the philospher/client in Chopstick Request Queue
		}
		else if(_readPtr == "F"){ //if the client/philosopher is done eating
			enqueuePhilosopherInChopStickReleasesQueue(_philTemp); //push the philosopher/Client in ChopStick Release Queue
		}
		
		sleep(GENERAL_SLEEP_TIME);
	}
	
	close(_clientSocketFd); //close client socket file descriptor
}


/*
 * THis function is a pthread start routine which is used to handle communication with a client/philosopher 
 * @param _args client socket descriptor struct
 * @return none
 */
void * startPhilosophers(void * _args){
	
	//Socket descriptor for client connection
	int _clientSocketFd;
	
	//Guarantees that thread resources are deallocated upon return
	pthread_detach(pthread_self()); 
	
	//Extract socket file descriptor from argument
	_clientSocketFd =  (( ClientFd *)_args)->_fd; //get client socket descriptor
	free(_args); //free allocaed memory for client socket struct
		
	handleClientConnection(_clientSocketFd);//start communication with client
}


/*
 * This function handles coordinator thread suck as granting/revoking chopstick from/to client/philosopher
 * @return none
 */
void handleCoordinator(){
	
	for(;;){ //initite loop for continuos connection
		
		//Release unused chopsticks
		if(isChopstickReleasesQueueEmpty() == false){ //if chopstick request queue is not empty
			int _len = getLengthChopStickReleasesQueue(); //get lenngth of chopstick request queue
			for(int i =0; i < _len ; i++){
				Philosopher _tempPhil = getFrontChopStickReleasesQueue(); //get the first Philosopher in the release queue
				
				_chopstickCollection[_tempPhil.getLeftChopStick()]= true; //free left chop stick
				_chopstickCollection[_tempPhil.getRightChopStick()]= true; //free right chop stick
				
				char _buf[200];
				//sprintf(_buf,"CentralizedServer: Philosopher(%d) has dropped Chopsticks%d and %d at Time: %d",_tempPhil.getId(),_tempPhil.getLeftChopStick(),_tempPhil.getRightChopStick(),(int)time(NULL));
				
				sprintf(_buf,"CentralizedServer: Philosopher(%d) has dropped Chopsticks%d and %d",_tempPhil.getId(),_tempPhil.getLeftChopStick(),_tempPhil.getRightChopStick());
				displayString(_buf); //print whcihc philosopher dropped which chopsticks
				
				dequeuePhilosopherInChopStickReleasesQueue(); //pop the philosopherfrom the queue
				
				sleep(1);
			}			
		}
		
		usleep(GENERAL_USLEEP_TIME);
		//sleep(GENERAL_SLEEP_TIME);
			
		//Grant Chopsticks to philosophers waiting in request Queue
		if(isChopStickRequestsQueueEmpty() == false){ //if the request quque 
			
			int _len = getLengthChopStickRequestsQueue(); //get length of chopstick release queue
			
			for(int i = 0; i < _len ; i++){
				
				Philosopher _tempPhil = getFrontChopStickRequestQueue(); //get the top item in the choptick release queue
				
				bool _isLeftFree = _chopstickCollection[_tempPhil.getLeftChopStick()]; //chcek to see if the chopstick index is free at the chostick collection
				bool _isRightFree = _chopstickCollection[_tempPhil.getRightChopStick()];//check another chopstick index 
				
				if((_isLeftFree == true) && (_isRightFree == true)){ //if requested chopsticks are available
					_chopstickCollection[_tempPhil.getLeftChopStick()]= false; //lock left chop stick
					_chopstickCollection[_tempPhil.getRightChopStick()]= false; //lock right chop stick
					
					char _buf[200];
					//sprintf(_buf,"CentralizedServer: Philosopher(%d) has pickedup Chopsticks%d and %d at Time: %d",_tempPhil.getId(),_tempPhil.getLeftChopStick(),_tempPhil.getRightChopStick(),(int)time(NULL));
					
					sprintf(_buf,"CentralizedServer: Philosopher(%d) has pickedup Chopsticks%d and %d",_tempPhil.getId(),_tempPhil.getLeftChopStick(),_tempPhil.getRightChopStick());
					displayString(_buf); //display which philosopher picked up which chosticks
					
					//send chopsticks
					char _sendBuf[WRITE_BUF_SZIE];
					sprintf(_sendBuf,"GRANT");
					int _err = send(_tempPhil.getFd(),_sendBuf,WRITE_BUF_SZIE,0); //indicate chopstick grant to client					
					if(_err == -1){
						perror("CentralizedServer: Error In Send");
						exit(4);
					}

					//sleep(GENERAL_SLEEP_TIME);
						
					
					dequeuePhilosopherInChopStickRequestsQueue(); //since the chopstick was grant to this clint.philosopher, remove this fro reuest queue
				}
				else{
					dequeuePhilosopherInChopStickRequestsQueue(); //pop the philosopher from the  reuest queue
					enqueuePhilosopherInChopStickRequestsQueue(_tempPhil); //enque at the bootom of the quque
					
					//char _buf[200];
					//sprintf(_buf,"CentralizedServer: Philosopher(%d): waiting in Line. Required Chopsticks Currrently  Not Available \n",_tempPhil.getId());
					//displayString(_buf);
				}
				
				//sleep(GENERAL_SLEEP_TIME);
			
			}
		}
		
		sleep(GENERAL_SLEEP_TIME);
	}	
}

/*
 * This fucntion is the srat routine for coordinator pthread, it handle all coordinator jobs
 * @param _args NULL
 * @return none
 */
void * startCoordinator(void * _args){
	pthread_detach(pthread_self()); //Guarantees that thread resources are deallocated upon return
	handleCoordinator(); //communicate with client communication
	return(NULL);
}


/*
 * This function accepts client connects for given server file descriptor and return clien socke descriptor upon sucess
 * @param _serverSocket server socket file descripto
 * @return _clientSocet descriptor uopon sucessful connection
 */
int acceptClientConnection(int _serverSocket){
	//acept clinet connections
	//int accept(int socketfd,
	//struct sockaddr *restrict address,
	// socklent *restrict address_len);
	int _clientSocket = accept(_serverSocket,NULL,NULL); //accept client connection
	if(_clientSocket == -1){
		perror("CentralizedServer: Could Not Accept Client Connection");
		exit(4);
	}
	return _clientSocket; //return client coket descriptor
}

/*
 * This fnction creatoes a TCP server socket and return the server socket file descriptor
 */
int createTcpServerSocket(){
	
	//create socket descriptor
	//arg1: Protocol Families : AF_INET (IPV4 Internet Protocols)
	//arg2: SOCK_STREAM : Connection Oriented socket
	//arg3: 0 : protocol: system will select protocol
	//based on protocol family (AF_INET)
	int _serverSocket = socket(AF_INET, SOCK_STREAM, 0);//create server socket
	if(_serverSocket == -1){
		perror("CentralizedServer: Could Not Create Socket Descriptor");
		exit(1);
	}
	else{
		printf("CentralizedServer: Socket File descriptor created\n");
	}

	//assign a name to Socket
	struct sockaddr_in _serverAddress;
	_serverAddress.sin_family = AF_INET; //protocl family
	_serverAddress.sin_port = htons(9112); //sever port number
	_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); //server address

	//bind socket to specified IP and port
	int _err = bind(_serverSocket, (struct sockaddr*)&_serverAddress,sizeof(_serverAddress)); //blind socket descriptor to name
	if(_err == -1){
		perror("CentralizedServer: Could not Not Bind Socket to Specified Address");
		exit(2);
	}
	else{
		printf("CentralizedServer: Socket Fd Bind Sucessful\n");
	}	

	//listent to client connections
	//int listen(int sockfd, int backlog)
	//sockfd: socket Descriptor
	//backlor: maximum length wo which
	//queue of pending connections for
	//sockfd may grow
	_err = listen(_serverSocket,NUMBER_OF_PHILOSOPHERS); //listen to client connections
	if(_err == -1){
		perror("CentralizedServer: Could Not Initiate Listen");
		exit(3);
	}
	else{
		printf("CentralizedServer: Server is now ready to listen\n");
	}

	return _serverSocket; //return server socket file descriptor
}


/*
 * This starting point of the program
 */
int main(int _argc, char *_argv[]){ 

	//initialize chopsticks
	int i = 0;
	for(i = 0; i < NUMBER_OF_PHILOSOPHERS; i++){
		_chopstickCollection[i] = true; //intiialilly make all chopsticks available
	}

	//create server socket
	int _serverSocketFd = createTcpServerSocket();
	
	//create pthread type for coordinator
	pthread_t _coordinator;
		
	//create pthread types for philosophers,
	pthread_t _philosopherList[NUMBER_OF_PHILOSOPHERS];	
	

	//start coordinator
	int _err = pthread_create(&_coordinator,NULL,startCoordinator,NULL);
	if(_err == -1){
		perror("CentralizedServer: COuld Not create and start coordinator thread");
	}
	
	int _philosopherId = 0;
	for(;;){
		//wait for client connection
		int _clientSocketFd = acceptClientConnection(_serverSocketFd);
		
		//Create separate memory for client argument
		ClientFd* _cfd = (ClientFd *)malloc(sizeof(ClientFd));
		_cfd->_fd = _clientSocketFd;

		//start seperate thread for this client
		_err = pthread_create(&_philosopherList[_philosopherId],NULL,startPhilosophers,(void *)_cfd);
		
		//increase count
		_philosopherId++;
		
		if(_philosopherId > NUMBER_OF_PHILOSOPHERS){
			perror("CentralizedServer: Error!: Client EXCEEDED 5. About to exit");
			exit(15);
		}
	}
	
	//join coordinator back to main thread
	pthread_join(_coordinator,NULL);

	//join all client thread back to main thread
	for(i = 0; i < _philosopherId; i++){
		pthread_join(_philosopherList[i],NULL);
		
	}

	return 0;
}
