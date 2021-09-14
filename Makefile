all:
	gcc -o vad main.c wb_vad.c -lm -g
clean:
	rm -f vad
