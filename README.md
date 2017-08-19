# SATA-COMMANDS


ALL THE CODE HAVE BEEN WRITTEN FOR LINUX(UBUNTU 16.04). They are using SCSI drivers for launching the SATA COMMANDS. It is standard in LINUX as SCSI is quite stable and has been there for long. SCSI has translation layers for ATA. It also expose ATA PASS THROUGH command for commands which are not recognized by it.   

"identify.c" code sends and interpret SATA IDENTIFY command.

"read.c" code sends SCSI READ command to an ATA Drive. for ATA PASS THROUGH method see the repository.

"write.c" code sends SCSI WRITE command to and ATA Drive. for ATA PASS THROGH method - see the repository.

"read_async.c" code shows the asunchronous method of lunching a command.

similarly "write_async.c" code.

for SMART LOG code see the repositry. 


COMPILING:  gcc file.c
EXECUTING: sudo ./a.out /dev/sg<>
ALL CODES ARE INDEPENDENT AND HAS BEEN KEPT SIMPLE.

LINK TO MY BLOG:
https://wisesciencewise.wordpress.com/

THere I have explained the code in STEPS. 
