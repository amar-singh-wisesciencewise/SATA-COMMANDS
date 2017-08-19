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
#include<linux/types.h>

//#define DISPLAY

#define IDENTIFY_REPLY_LEN 512



int main(int argc, char* argv[]){
	int fd,i;

	unsigned char identify_cdb[12] = 
		{0xa1,0x0c,0x0d,1,1,0,0,0,0,0xec,0,0};


	sg_io_hdr_t io_hdr;
	char* file_name = 0;
	__u16 buffer[IDENTIFY_REPLY_LEN];//since identify give data in wordsize
	unsigned char sense_buffer[32];

	if(argc < 2){
		printf("please enter a device file\n");
		return 1;
	}else{
		file_name = argv[1];
	}
	
	/////////opening the device file/////////////

	if((fd = open(file_name,O_RDWR))<0){
		printf("device file opening failed\n");
		return  1;
	}
	

	/////////// data buffer ///////////
//	printf("********data buffer initially***********\n");
	for(i=0 ; i< (IDENTIFY_REPLY_LEN) ; i++){
		buffer[i] = 0;
//		printf("%hx ",buffer[i]);
	}
	printf("\n");
	
	

	
	////////////////prepare read//////
	memset(&io_hdr,0,sizeof(sg_io_hdr_t));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = sizeof(identify_cdb);
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = (IDENTIFY_REPLY_LEN);
	io_hdr.dxferp = buffer;
	io_hdr.cmdp = identify_cdb;
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = 20000;


	if(ioctl(fd,SG_IO,&io_hdr)<0){
		printf("ioctl failed\n");
		for(i = 0 ; i<32;i++)
			printf("%hx ",sense_buffer[i]);
		return 1;
	}
	close(fd);
	
#ifdef DISPLAY	
	printf("********data buffer after ioctl***********\n");
	for(i=0; i<(IDENTIFY_REPLY_LEN) ; i++){
		printf("%hx ",buffer[i]);
	}
#endif
	printf("\n");

	printf("SATA IDENTIFY DETAILS\n");
	
	if(buffer[75]&0x001f)
		printf("Queue depth supported\nQueue Depth: %d\n",(int)(buffer[75]&0x001f)+1);//+1 since queue depth reported is 1 less than actual value

	if(buffer[85]&0x0020)	
		printf("volatile write cache is enabled\n");
	
	if(buffer[85]&0x2000)	
		printf("Read Buffer command is supported\n");
	
	if(buffer[85]&0x1000)	
		printf("Write Buffer Command is supported\n");
	
	if(buffer[86]&0x0400)	
		printf("48 bit address features set is supported\n");
	
	if(buffer[86]&0x2000)	
		printf("Flush Cache Ext command is supported\n");

	if(buffer[86]&0x1000)	
		printf("Flush Cache command is supported\n");
	
	if(buffer[69]&0x0008){
		printf("Extenden number of user addressable LBAs: %lld\n", *((__u64*)(buffer+230)));
	}	
	
	printf("logical sector size: %d\n", *((__u32*)(buffer+117)));
	printf("user addressable lbas[60 word]: %d\n", *((__u32*)(buffer+60)));
	printf("user addressable lbas[100 word]: %lld\n", *((__u64*)(buffer+100)));

	if((buffer[106]&0xc000) == 0x4000)
	{
		if(buffer[106]&0x2000) printf("device has multiple logical sectors per physical sectors\n");
		if(buffer[106]&0x1000) printf("Device Logical Sector longer than 512B\n");
			else printf("logical sector size is 512B\n");
		printf("logical sectors per physical sector (2^): %d\n",(int)(buffer[106]&0x000f));
	}

	

	printf("\n*********duration = %d\n", io_hdr.duration);

return 0;
}

