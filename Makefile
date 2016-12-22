# 
# VERSION CHANGES
#

DESTDIR=/usr/local
CFLAGS=-Wall -g -I. -O2 
LIBS=-lsqlite3

OBJ=imessage2txt
OFILES=
default: imessage2txt

.c.o:
	${CC} ${CFLAGS} $(COMPONENTS) -c $*.c

all: ${OBJ} 

imessage2txt: ${OFILES} imessage2txt.c 
#	ctags *.[ch]
#	clear
	${CC} ${CFLAGS} $(COMPONENTS) imessage2txt.c ${OFILES} -o imessage2txt ${LIBS}

install: ${OBJ}
	cp imessage2txt ${DESTDIR}/bin/

clean:
	rm -f *.o *core ${OBJ}
