all: centralCoord

centralCoord:
	cc -o ./bin/startC centralCoord.c
	cc -o ./bin/philosopher philosopher.c

startC:
	./bin/startC 31200

clean:
	rm -f ./bin/startC
	rm -f ./bin/philosopher