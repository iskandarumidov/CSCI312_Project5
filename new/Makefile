all: kp clean current

current:
	cc -Wno-macro-redefined starter.c -o ./bin/starter
	cc -Wno-macro-redefined philosopher.c -o ./bin/philosopher -pthread
	cc -Wno-macro-redefined coordinator.c -o ./bin/coordinator -pthread

kp:
	-kill $$(lsof -t -i :31200 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31201 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31202 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31203 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31204 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31205 -s TCP:LISTEN) 2>/dev/null ; true

netstat:
	netstat -anvp tcp | awk 'NR<3 || /LISTEN/'

clean:
	rm -f ./starter
	rm -f ./philosopher
	rm -f ./coordinator
