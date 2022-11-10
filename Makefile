all: clean centralCoord philosopher tokenRingAlgoirthm starter

centralCoord: soc.h philosopher.h centralCoord.c
	$(CC) -o ./bin/startC philosopher.h soc.h centralCoord.c

philosopher: soc.h philosopher.h philosopher.c
	$(CC) -o ./bin/philosopher philosopher.h soc.h philosopher.c

tokenNode: soc.h philosopher.h tokenRingAlgoirthm.c
	$(CC) -o ./bin/tokenRingAlgoirthm philosopher.h soc.h tokenRingAlgoirthm.c

starter: soc.h philosopher.h tokenRingStarter.c
	$(CC) -o ./bin/startTR philosopher.h soc.h tokenRingStarter.c

clean:
	rm -f ./bin/startC
	rm -f ./bin/startTR
	rm -f ./bin/philosopher
	rm -f ./bin/tokenRingAlgoirthm