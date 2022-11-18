all: kp current

current:
	cc -Wno-macro-redefined server.c -o server
	cc -Wno-macro-redefined philosopher.c -o philosopher
	cc -Wno-macro-redefined starter.c -o starter

kp:
	-kill $$(lsof -t -i :31200 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31201 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31202 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31203 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31204 -s TCP:LISTEN) 2>/dev/null ; true
	-kill $$(lsof -t -i :31205 -s TCP:LISTEN) 2>/dev/null ; true

clean:
	rm -f ./server
	rm -f ./philosopher
	rm -f ./starter
