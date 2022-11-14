all: kp clean current

current:
	cc -Wno-macro-redefined starter.c -o starter
	cc -Wno-macro-redefined philosopher.c -o philosopher

kp:
	-kill $$(lsof -t -i :31200 -s TCP:LISTEN)
	-kill $$(lsof -t -i :31201 -s TCP:LISTEN)
	-kill $$(lsof -t -i :31202 -s TCP:LISTEN)
	-kill $$(lsof -t -i :31203 -s TCP:LISTEN)
	-kill $$(lsof -t -i :31204 -s TCP:LISTEN)
	-kill $$(lsof -t -i :31205 -s TCP:LISTEN)

clean:
	rm -f ./starter
	rm -f ./philosopher
