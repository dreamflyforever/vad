all:
	gcc -o vad main.c wb_vad.c -lm
clean:
	rm -f vad
