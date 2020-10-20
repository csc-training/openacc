CC=pgc++
CFLAGS=-fast -ta=tesla:managed:cc70 -Minfo=accel

all: 
	${CC} ${CFLAGS} main.cpp -o cg
clean:
	rm -Rf cg pgprof* *.o core
