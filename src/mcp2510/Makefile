CC=/opt/host/armv4l/bin/armv4l-unknown-linux-gcc
LD=/opt/host/armv4l/bin/armv4l-unknown-linux-ld
CFLAGS = -D__KERNEL__ -I/mnt/my/HHARM9-EDU/kernel/include/linux -I/mnt/my/HHARM9-EDU/kernel/include -Wall -Wstrict-prototypes -Wno-trigraphs -Os -mapcs -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -DKBUILD_BASENAME=s3c2410_testirq -I/opt/host/armv4l/src/linux/include -DMODULE	

can_all.o: can.o 
	$(LD) -r $^ -o $@ 
all: can_all.o
 
clean:
	rm -rf *.o
