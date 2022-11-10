all: centralCoord

centralCoord:
	cc -o ./bin/startC centralCoord.c
	cc -o ./bin/philosopher philosopher.c
	cc -o ./bin/tokenRingAlgoirthm tokenRingAlgoirthm.c
	cc -o ./bin/startTR tokenRingStarter.c

clean:
	rm -f ./bin/startC
	rm -f ./bin/startTR
	rm -f ./bin/philosopher
	rm -f ./bin/tokenRingAlgoirthm