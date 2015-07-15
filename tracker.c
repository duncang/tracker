#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


int run = 1;

#define RBSIZE 255


void handleSignal(int sig)
{

	fprintf(stderr,"Got signal (%d): Quitting\n", sig);
	run = 0;
}

int main(int argc, char const *argv[])
{
	
	int iGPSPort;
	char szGPSPort[255];


	// setup signals
	signal(SIGINT,handleSignal);


	sprintf(szGPSPort,"/dev/tty.usbserial");


	fprintf(stdout,"Opening %s\n",szGPSPort);
	iGPSPort = open(szGPSPort,O_RDWR|O_NOCTTY|O_NONBLOCK);

	if (iGPSPort < 0)
	{

		fprintf(stderr,"Failed to open GPS Port: %s\n",strerror(errno));
		run = 0;
	}


	// setup serial port
	struct termios termPortSettings;
	memset (&termPortSettings,0,sizeof(struct termios));
	cfmakeraw(&termPortSettings);
	cfsetspeed(&termPortSettings, B4800);
	termPortSettings.c_cflag = CREAD | CLOCAL | CS8;
	termPortSettings.c_iflag
	termPortSettings.c_oflag
	termPortSettings.c_cc[VMIN] = 0;
	termPortSettings.c_cc[VTIME] = 10;
	tcsetattr(iGPSPort,TCSANOW,&termPortSettings);

	while (run == 1)
	{
		int iBytesRead;
		char szReadBuffer[RBSIZE];  // ring buffer
		int iReadIndex = 0,iWriteIndex = 0; // ring buffer read and write index;

		// read input
		iBytesRead = read(iGPSPort, &szReadBuffer[iWriteIndex%RBSIZE],1);

		if (iBytesRead > 1) fprintf(stderr,"ERROR: Returned more bytes (%d) than expected!\n", iBytesRead);

		if(iBytesRead > 0)
		{
			iWriteIndex += iBytesRead; 
			fprintf(stdout,"Read (%d) bytes: \n",iBytesRead);

			processBytes();
		}

		

		// just slow everything down a bit
		sleep(1);



	}


	fprintf(stdout,"Closing GPS Port\n");
	close(iGPSPort);

	return 0;
}