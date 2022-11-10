all: clean centralCoord philosopher tokenRingAlgorithm starter

centralCoord: soc.h philosopher.h centralCoord.c
	$(CC) -o ./bin/startC centralCoord.c

philosopher: soc.h philosopher.h philosopher.c
	$(CC) -o ./bin/philosopher philosopher.c

tokenNode: soc.h philosopher.h tokenRingAlgorithm.c
	$(CC) -o ./bin/tokenRingAlgorithm tokenRingAlgorithm.c

starter: soc.h philosopher.h tokenRingStarter.c
	$(CC) -o ./bin/startTR tokenRingStarter.c

clean:
	rm -f ./bin/startC
	rm -f ./bin/startTR
	rm -f ./bin/philosopher
	rm -f ./bin/tokenRingAlgorithm