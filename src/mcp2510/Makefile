CC=~/embedded_linux_tmp/arm_tools/bin/arm-linux-gcc
LD=~/embedded_linux_tmp/arm_tools/bin/arm-linux-ld
CFLAGS = -D__KERNEL__ -I/home/b092837_linxuankang/embedded_linux_tmp/source_dir/linux-2.4.18_kernel/include/linux -I/home/b092837_linxuankang/embedded_linux_tmp/source_dir/linux-2.4.18_kernel/include -Wall -Wstrict-prototypes -Wno-trigraphs -Os -mapcs -fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -DKBUILD_BASENAME=s3c2410_testirq -I/home/b092837_linxuankang/embedded_linux_tmp/source_dir/linux-2.4.18_kernel/include/asm/arch -DMODULE 


can_all.o: can.o 
	$(LD) -r $^ -o $@ 

all: can_all.o test send receive

test:
	$(CC) -o test.o test.c

send:
	$(CC) -o send.o send.c
 
receive:
	$(CC) -o receive.o receive.c


final: mcu door light

mcu:
	$(CC) -o mcu.o mcu.c

door:
	$(CC) -o door.o door.c

light:
	$(CC) -o light.o light.c


clean:
	rm -rf *.o
