EXECS=sort2d
MPICC?=mpicc

all: ${EXECS}

sort2d: sort2d.c 
	${MPICC} -O3 -o $@ sort2d.c

clean:
	rm ${EXECS}