# makfile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h
LDFLAGS		= -L../lib -L/opt/ti/msp430_gcc/include/ 

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-as -mmcu=${CPU} -c
#AR		= msp430-elf-ar

all:lab3_main.elf

#additional rules for files
lab3_main.elf: ${COMMON_OBJECTS} lab3_main.o wdt_handler.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer -lLcd 

load: lab3_main.elf
	msp430loader.sh $^

clean:
	rm -f *.o *.elf

#lcdutils.o: lcdutils.c lcdutils.h
#lcddraw.o: lcddraw.c lcddraw.h lcdutils.h

#libLcd.a: font-11x16.o font-5x7.o font-8x12.o lcdutils.o lcddraw.o
#	$(AR) crs $@ $^

#install: libLcd.a
#	mkdir -p ../h ../lib
#	mv $^ ../lib
#	cp *.h ../h
