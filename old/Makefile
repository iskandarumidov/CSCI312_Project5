all: clean centralCoord philosopher tokenRingAlgoirthm starter

centralCoord: soc.h philosopher.h centralCoord.c
	$(CC) -o startC centralCoord.c

philosopher: soc.h philosopher.h philosopher.c
	$(CC) -o philosopher philosopher.c

tokenNode: soc.h philosopher.h tokenRingAlgoirthm.c
	$(CC) -o tokenRingAlgoirthm tokenRingAlgoirthm.c

starter: soc.h philosopher.h tokenRingStarter.c
	$(CC) -o startTR tokenRingStarter.c

clean:
	rm -f startC
	rm -f startTR
	rm -f philosopher
	rm -f tokenRingAlgoirthm