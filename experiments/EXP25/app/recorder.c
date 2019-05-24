//#define MYDEBUG
#include "myhead.h"

#define BASEBUFSIZE 1000*180
#define BUFSIZE 5000
#define IOCTL_LINEIN 0x27
#define IOCTL_MIC	0x28

char magic[]={0x52,0x49,0x46,0x46};
char middle[]={0x57,0x41,0x56,0x45,0x66,0x6D,0x74,0x20,
			   0x10,0x00,0x00,0x00,0x01,0x00,0x02,0x00,
			   0x44,0xAC,0x00,0x00,0x10,0xB1,0x02,0x00,
			   0x04,0x00,0x10,0x00,0x64,0x61,0x74,0x61};

char rxbuf[BUFSIZE];
void amplify(float mag, void *buf, int count);

void write_wavheader(int fd,int data_len)
{
	int tmp;
	write(fd,magic,4);
	tmp=data_len+36;
	write(fd,(char*)&tmp,4);
	write(fd,middle,32);
	tmp=data_len;
	write(fd,(char*)&tmp,4);
}

/* amplify the sound, try to avoid float point calculation */
/* mag: magnitude of amplification */
void amplify(float mag, void *buf, int count)
{
	int realmag, realmag2;
	int i, wcount;
	short *wbuf;

	wbuf = (short *) buf;
	wcount = count / 2;

	if (mag >= 1) {
		/* volume + */
		realmag = mag;
		for (i = 0; i < wcount; i++)
			wbuf[i] *= realmag;
	} else if (mag < 1 && mag > 0) {
		realmag2 = 1 / mag;
		for (i = 0; i < wcount; i++)
			wbuf[i] /= realmag2;
	}
}


int main(int argc, char **argv)
{
	//id: DEVICE ID; fd: Recorded file
	int rec_fd, out_fd;
	//int play_fd;
	int i, n, m, tmp;
	int cnt;

	//Options 
	int output, test;
	//int filesize;
	//char *playbuf;
	char *filename;
	char *rec_dev, *play_dev;

	test = 0;
	output = 1;
	cnt = 1;
	filename = "/tmp/out.wav";
	rec_dev = "/dev/sound/mixer";	
	play_dev = "/dev/dsp";

	/* open rec_dev file and output file */
	if ((rec_fd = open(rec_dev, O_RDWR)) <= 0) {
		fprintf(stderr, " Can't open record device %s! errno=%d\n",
			rec_dev, errno);
		fflush(stdout);
		exit(-1);
	}

	while ((tmp = getopt(argc, argv, "o:thmln:")) != -1) {
		switch (tmp) {
		case 'o':
			output = 1;
			filename = optarg;
			break;
		case 'm':
			printf("....user set mic.\n");
			ioctl(rec_fd,IOCTL_MIC,0);
			break;
		case 'l':
			printf("....user set linein.\n");
			ioctl(rec_fd,IOCTL_LINEIN,0);
			break;
		case 't':
			test = 1;
			break;
		case 'h':
			fprintf(stderr, "Usage: %s [-r] [-t] [-o file] \n",
				argv[0]);
			break;
		case 'n':
			cnt = strtol(optarg, NULL, 10);
			break;
		default:
			printf("Unrecognized parameter \"-%c\"!\n",
			       optopt);
		}
	}

	if ((out_fd =
	     open(filename, O_RDWR | O_CREAT | O_TRUNC), 0777) <= 0) {
		fprintf(stderr, " Can't open output file! errno=%d\n",
			errno);
		goto end;
	}
/*
        if ((play_fd = open(play_dev, O_WRONLY,0777)) <= 0) {
                fprintf(stderr,
                        " Can't open playback device %s! errno=%d\n",
                        play_dev, errno);
                close(out_fd);
                goto end;
        }	
*/	
	cnt = cnt * (BASEBUFSIZE / BUFSIZE);

	//PDEBUG("Reset buffer.\n");
	memset(rxbuf, 0, BUFSIZE);

	/*write au header*/
	write_wavheader(out_fd,cnt*BUFSIZE);

	/* record sound */
	printf("Recording...(output file:%s)\n", filename);
	for (i = 0; i<cnt; i++) {
		PDEBUG
			("recorder read from rec_dev(rec_fd=%d,rxbuf=%p,count=%d)\n",
			 rec_fd, (void *) &rxbuf, BUFSIZE);
		if ((n = read(rec_fd, rxbuf, BUFSIZE)) != BUFSIZE)
			printf
			    ("ERROR:read returned count 0x%x, errno:%d\n",
			     n, errno);
		PDEBUG
		    ("recorder write to file(out_fd=%d,rxbuf=%p,count=%d)\n",
		     out_fd, (void *) &rxbuf, n);

		//write(play_fd, rxbuf, BUFSIZE); //play

		if ((m = write(out_fd, rxbuf, BUFSIZE)) != BUFSIZE)
			printf
			    ("ERROR:write(%s) return count: 0x%d, errno:%d\n",
			     filename, m, errno);
	}			//endfor
	PDEBUG("closing output file...\n");
	close(out_fd);

#if 0
	//PAUSE();
	/* open the 16bit, 44.1kHz PCM file. Play it for a test */
	if (test) {
		filesize = cnt*BUFSIZE + 44;

		if(playbuf=malloc(filesize)==NULL){
			printf("Memory not enough for test.\n");
			goto end;
		}
		bzero(playbuf,filesize);

		if ((out_fd = open(filename, O_RDONLY)) <= 0) {
			fprintf(stderr,
				" Can't open output file!, errno=%d\n",
				errno);
			goto end;
		}

		if ((play_fd = open(play_dev, O_WRONLY,0777)) <= 0) {
			fprintf(stderr,
				" Can't open playback device %s! errno=%d\n",
				play_dev, errno);
			close(out_fd);
			goto end;
		}

		printf("\nBegin to play %s for test.\n", filename);
		//n = read(out_fd, playbuf, filesize);
		close(out_fd);

		//PDEBUG("Volume - \n");
		//amplify(0.1, rxbuf,n);

		PDEBUG
		    ("Test write to play_dev(play_fd=%d,rxbuf=0x%p,count=%d)\n",
		     play_fd, playbuf, filesize);
		//write(play_fd, playbuf, filesize);
		write(play_fd, rxbuf, BUFSIZE);

		close(play_fd);
		PDEBUG("Test finished.\n");
	}
#endif

      end:

	fflush(stdout);
	fflush(stderr);
	close(rec_fd);
	printf("\nRecord finished.\n");
	return 0;
}

