#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "vad_api.h"

FILE *fp_all;

int vad_cb(void *arg, int flag)
{

	if (fp_all != NULL) {
		fclose(fp_all);
		fp_all = NULL;
	}
	vad_log("flag: %d\n", flag);
	return 0;
}

int main(int argc, char *argv[])
{
	int retval;

	vad_str *vad;
	vad_init(&vad, vad_cb, NULL);
	/*1 second*/
	vad_settime(vad, 80);
	
	int rc = 1;
	int frames = FRAME_LEN;
	int in_fd = open(argv[1], O_RDONLY, 0);
	char *buffer = (char*)malloc(frames * 2);

	fp_all = fopen("second", "wb");
	while (rc > 0) {
		rc = read(in_fd, buffer, frames);
		retval = vad_feed(vad, buffer, frames);
		if (retval == FAIL) {
			vad_log("vad feed fail\n");
			goto out;
		}
		if (fp_all != NULL)
			fwrite(buffer, 1, frames, fp_all);
	}
	free(buffer);
	if (fp_all != NULL)
		fclose(fp_all);
	close(in_fd);
	vad_delete(&vad);
out:
	return 0;
}
