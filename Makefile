all:
	gcc -o vad main.c wb_vad.c vad_api.c -lm -g -I.
clean:
	rm -f vad
