#include <unistd.h>
#include "Pblock.hpp"

Pblock::Pblock(port_t port, int numports, int * fd, bool * isinitialized) : Periph(port, numports, fd, isinitialized) {}

int Pblock::read(int loc, void * buf, int nbyte){
	lseek(fd[port_], loc, SEEK_SET);
	return read(buf, nbyte);
}

int Pblock::write(int loc, const void * buf, int nbyte){
	lseek(fd[port_], loc, SEEK_SET);
	return write(buf, nbyte);
}
