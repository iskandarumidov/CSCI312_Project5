all: serverCentralized clientCentralized sc 

serverCentralized: CentralizedServer.cpp Philosopher.h
	g++ -std=c++0x -pthread CentralizedServer.cpp Philosopher.h -o serverCentralized

clientCentralized: CentralizedClient.cpp Philosopher.h 
	g++ CentralizedClient.cpp Philosopher.h  -o clientCentralized
	
sc: StartClient.cpp
	g++ StartClient.cpp -o sc

clean:
	rm -f serverCentralized
	rm -f clientCentralized
	rm -f sc
	rm -f output.txt

