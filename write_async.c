#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<error.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<scsi/scsi_ioctl.h>
#include<scsi/sg.h>



#define WRITE16_LEN  512
#define WRITE16_CMD_LEN 16
#define BLOCK_SIZE 126

int main(int argc, char* argv[]){
	int fd,i;
	int lba;
	printf("Enter the LBA you want to write to:  ");
	scanf("%d",&lba);
	
	if(lba>=256){
		printf("LBA greater than 512 is not allowed with this version: ");
		return 1;
	}
	unsigned char w16CmdBlk[WRITE16_CMD_LEN] = 
			{0x8A,0,0,0,0,0,0,0,0,lba,0,0,0,BLOCK_SIZE,0,0};
	sg_io_hdr_t io_hdr;
	char* file_name = 0;
	unsigned char buffer[WRITE16_LEN*BLOCK_SIZE];
	unsigned char sense_buffer[32];

	if(argc < 2){
		printf("please enter a device file\n");
		return 1;
	}else{
		file_name = argv[1];

		if(file_name[7] == 'a' ){
			printf("WARNING: do not write on /dev/sda device\n");
			return 1;
		}
	}

	/////////opening the device file/////////////

	if((fd = open(file_name,O_RDWR))<0){
		printf("device file opening failed\n");
		return  1;
	}
	

	/////////// data buffer ///////////
	
	printf("********data buffer initializing***********\n");
	for(i=0;i<512*BLOCK_SIZE;i++){
		buffer[i] = rand()%256;
	}
	buffer[2] = 'A';
	buffer[3] = 'M';
	buffer[4] = 'A';
	buffer[5] = 'R';
	
	printf("\n");
	
/*	
	printf("********data buffer after initialling***********\n");
	for(i=0;i<512;i++){
		printf("%hx ",buffer[i]);
	}
	printf("\n");

*/	
	////////////////prepare read//////
	memset(&io_hdr,0,sizeof(sg_io_hdr_t));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(w16CmdBlk);
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.dxfer_direction = SG_DXFER_TO_DEV;
	io_hdr.dxfer_len = WRITE16_LEN*BLOCK_SIZE;
//	io_hdr.dxfer_len = WRITE16_LEN;
	io_hdr.dxferp = buffer;
	io_hdr.cmdp = w16CmdBlk;
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = 20000;



///////////////////////////write and read method/////////////
	int ret = 0;
	ret = write(fd,&io_hdr,sizeof(io_hdr));
	if(ret == sizeof(io_hdr)){
		printf("\nwrite was successful\n");
		
	}else{
		printf("\nwrite failed \n");
		return 1;
	}

	printf("\nthese values are being printed just after write call\n");
	printf("duration: %d \n",io_hdr.duration);

	ret = read(fd,&io_hdr,sizeof(io_hdr));
	if(ret == sizeof(io_hdr)){
		printf("\nread was successful\n");
		
	}else{
		printf("\nread failed \n");
		return 1;
	}

	close(fd);

	printf("\nthese values are being printed just after read call\n");
	printf("duration: %d \n",io_hdr.duration);

/*
	if(ioctl(fd,SG_IO,&io_hdr)<0){
		printf("ioctl failed\n");
		for(i = 0;i<32;i++){
			printf("%hx ",sense_buffer[i]);
		}
		printf("\n");
		return 1;
	}else printf("write successfull\n");
	
	printf("duration:  %d\n",io_hdr.duration);	
*/	
return 1;
}

