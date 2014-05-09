#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "i2c-dev.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define I2C_ADDRESS		0x42
#define BUF_LENGTH		200
int fd;


int main (int argc, char **argv) {
	
	 
	int i;
	int fdm;
	int rc;
	int index;
	
	volatile unsigned int* tmp_ptr;
	volatile unsigned int* p;
	unsigned int offset;
	unsigned int value;
	char ret;
	char buf[BUF_LENGTH+1];
	char buf_input[BUF_LENGTH+1];	


	fdm = posix_openpt(O_RDWR);
	if (fdm < 0)
	{
		fprintf(stderr, "Error %d on posix_openpt()\n", errno);
		return 1;
	} 

	rc = grantpt(fdm);
	if (rc != 0)
	{
		fprintf(stderr, "Error %d on grantpt()\n", errno);
		return 1;
	}

	rc = unlockpt(fdm);
	if (rc != 0)
	{
		fprintf(stderr, "Error %d on unlockpt()\n", errno);
		return 1;
	} 

	printf("The slave side is named : %s\n", ptsname(fdm)); 

	fd=open("/dev/i2c-2", O_RDWR);
	if(fd <0){
		printf("open device\n");
		return -1;
	}
	
	if (ioctl(fd, I2C_SLAVE_FORCE, I2C_ADDRESS) < 0) {
		close(fd);
		printf("Cannot set slave address\n");
		return -1;
	}

	
	while(1)
	{	
		ret = 0;
		index = 0;
		while (index < BUF_LENGTH)
		{		
			ret=i2c_smbus_read_byte_data(fd,0xFF);
			if(ret<127)
			{
				buf[index] = ret;
				index++;	
			} else {
				break;			
			}
		}
		if (index > 0)
		{
			write(fdm,buf,index);	
			buf[index]=0;
			printf("%s",buf);		
		}
		usleep(10000);
	}
	
	close(fd);
	close(fdm);
	return 0;

}

