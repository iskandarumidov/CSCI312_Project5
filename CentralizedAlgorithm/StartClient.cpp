//This program is sued to start n number of clients

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

//start point of the program number of clients is to be specified here in argv[1]
int main(int _argc, char * _argv[]){
	
	if(_argc < 1){
		perror("StartClient: Need to specifiy Number of clients");
		exit(5);
	}
 
	int _totalClient = atoi(_argv[1]);
	
	if(_totalClient < 5){
		perror("StartClient: Need 5 pilosophers ");
		exit(6);
	}
	
	printf("Total Clients: %i\n",_totalClient);
	pid_t _pid;
	int i = 0;

	//Start child process process by _toalClient
	for(i = 0; i < _totalClient;i++){
		
		_pid = fork(); //creat child process

		if(_pid < 0){ //if fork() wasnt sucessful print error
			perror("StartClients: Could Not Fork");
			exit(0);
		}
		else if(_pid == 0){ //if in child process start a clint

			char _buf[10];
			sprintf(_buf,"%i",i);
			printf("About To Start Client: %i\n",i);
			int _err = execl("./clientCentralized","Test",_buf,(char *)NULL);
			if(_err == -1){
				perror("COuld Not Execute Execl");
				exit(1);
			}
		}
	}
	
	//wait for child process to terminate so that they dont become zombie processes 
	for(i = 0; i < _totalClient; i++){
		int _status = 0;
		int _err = waitpid(-1,&_status,0);
		if(_err == -1){
			perror("StartClient: Could Not perfrom waitpid. About to exit");
			exit(2);
		}
	}

	return 0;
}
