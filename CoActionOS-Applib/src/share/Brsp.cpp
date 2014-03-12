
#include <cstring>
#include <cstdio>
#include "Timer.hpp"

#include "Brsp.hpp"


void Brsp::flush(void){
	char buffer[32];
	Uart::flush();
	while( read(buffer, 32) > 0 ){
		;
	}
}

//change between data and command mode
int Brsp::setmode(mode_t mode, int handle){
	StringSmall command;
	int ret = -1;
	switch(mode){
	case MODE_COMMAND:
		//escape to command mode
		command << "+++\r";
		write(command.c_str(), command.size());
		return 0;
	case MODE_DATA:
		if( handle > -1 ){
			command << "ATMD," << handle << "\r";
		} else {
			command = "ATMD\r";
		}
		break;
	case MODE_REMOTE_COMMAND:
		if( handle > -1 ){
			command << "ATMRC," << handle << "\r";
		} else {
			command = "ATMRC\r";
		}
		break;
	default:
		ret = -1;
	}
	ret = sendcommand(command.c_str(), 0, 0);
	return ret;
}

int Brsp::setproperty(StringSmall cmd, StringSmall value){
	StringMedium command;
	bool ret;
	int tmp;
	command << cmd << "," << value << "\r";
	tmp = timeout;
	settimeout(200);
	ret = sendcommand(command);
	settimeout(tmp);
	return ret == 0;
}

StringSmall Brsp::rssi(int handle){
	StringSmall tmp;
	tmp << "ATRSSI?," << handle << "\r";
	return property(tmp);
}


StringSmall Brsp::property(StringSmall command){
	StringSmall ret;
	StringSmall tmp;
	if( sendcommand(command.c_str(), tmp.data(), tmp.capacity()) == 0 ){
		if( tmp.size() > 4){
			tmp.copy(ret.data(), tmp.size()-4, 2);
		}
	} else {
		ret = "FAILED";
	}
	return ret;
}

bool Brsp::setname(StringSmall name){
	return setproperty("ATSN", name);
}

bool Brsp::setuart(baudrate_t baudrate, parity_t parity, stopbits_t stopbits, flowcontrol_t flowcontrol){
	int tmp;
	int ret;
	StringMedium command;
	command << "ATSUART," << baudrate << "," << parity << "," << stopbits << "," << flowcontrol << "\r";
	tmp = timeout;
	settimeout(200);
	ret = sendcommand(command);
	settimeout(tmp);
	return ret;
}


StringMedium Brsp::listen(void){
	StringMedium ret;
	readline(ret.data(), ret.capacity(), timeout, '\r');
	return ret;
}

bool Brsp::event(StringSmall ev){
	StringMedium ret;
	StringSmall arrived;
	ret = listen();

	if( ret.size() > 0 ){
		ret.csv(arrived, 0);
		if( arrived == ev ){
			return true;
		}
	}

	return false;
}


typedef struct {
	BrspAddress * addrs;
	int state;
	int addr_offset;
	int max;
} discover_t;

static void discover_callback(void * arg, const void * buffer, int nbyte){
	const char * c;
	int i;
	discover_t * p = (discover_t*)arg;
	c = (const char *)buffer;

	for(i=0; i < nbyte; i++){
		switch( p->state ){
		case 0:
			//look for first comma
			if(*c == ',' ){
				p->state++;
			}
			break;
		case 1:
			//look for second comma
			if(*c == ',' ){
				p->state++;
			}
			break;
		case 2:
			//copy address to addr
			if( (*c != ',') && (p->addr_offset < p->max) ){
				p->addrs[p->addr_offset] << *c;
			} else {
				p->state++;
			}
			break;
		case 3:
			//look for newline
			if( *c == '\n' ){
				p->state = 0;
				p->addr_offset++;
			}
			break;
		}
		c++;
	}
}


//discover new devices
int Brsp::discover(BrspAddress addrs[], int max){
	discover_t arg;
	int tmp;
	memset(&arg, 0, sizeof(discover_t));
	int i;
	for(i=0; i < max; i++){
		addrs[i].clear();
	}
	arg.addrs = (BrspAddress*)addrs;
	arg.max = max;
	tmp = timeout;
	settimeout(500);
	if( sendcommand("ATDILE\r", discover_callback, &arg) < 0 ){
		return -1;
	}
	settimeout(tmp);
	Timer::wait_msec(10);
	flush();
	return arg.addr_offset;
}

//connect to a device
int Brsp::connect(const BrspAddress & addr, mode_t mode){
	int handle;
	StringMedium command;
	StringMedium element;
	command << "ATDMLE," << addr << "," << mode << "\r";
	if(  sendcommand(command.c_str()) != 0 ){
		return -1;
	}

	handle = -2;
	//wait for EVENTS
	command.clear();
	for(int i = 0; i < 6; i++){
		//parse response to find connection handle
		command.clear();
		readline(command.data(), command.capacity(), 1000, '\n');
		command.csv(element, 0);
		if ( element == "CONNECT" ){
			command.csv(element,1);
			handle = element.atoi();
		}

	}
	return handle;
}

int Brsp::set_duty(char led, uint16_t duty, uint16_t period){
	StringMedium str;
	str.setformat(StringUtil::DECIMAL);
	str << "ATSLED," << led << "," << duty << "," << period << "\r";
	return sendcommand(str.c_str(), 0, 0);
}

//connect to a device
int Brsp::disconnect(int handle){
	StringSmall command;
	command << "ATDH," << handle << "\r";
	return sendcommand(command.c_str(), 0, 0);
}

typedef struct {
	void * buf;
	int offset;
	int max;
} fill_buffer_t;

static void fill_buffer(void * arg, const void * buffer, int nbyte){
	int copy_bytes;
	fill_buffer_t * p = (fill_buffer_t *)arg;
	if( p->offset + nbyte < p->max ){
		copy_bytes = nbyte;
	} else if( p->offset < p->max ){
		copy_bytes = p->max - p->offset;
	} else {
		copy_bytes = 0;
	}
	memcpy((char*)(p->buf) + p->offset, buffer, copy_bytes);
	p->offset += copy_bytes;
}

bool Brsp::paired(void){
	StringSmall dest;
	dest = property("ATCS?\r");
	if( dest == "FAILED" ){
		return false;
	}
	return true;
}

int Brsp::sendcommand(const char * command, void * buffer, int max){
	fill_buffer_t arg;
	arg.buf = buffer;
	arg.max = max-1;
	arg.offset = 0;
	memset(buffer, 0, max);
	return sendcommand(command, fill_buffer, &arg);
}

//send a command
int Brsp::sendcommand(const char * command, void (*response)(void*, const void *,int), void * response_arg){
	int len;
	int err;
	char buffer[32];
	int t;
	StringSmall msg;

	//flush any data present in the receive buffer
	flush();

	//send the command
	len = strlen(command);
	if( write(command, len) != len ){
		//failed to write command
		return -1;
	}

	if( !wait("\r\nOK\r\n") ){
		return -1;
	}

	if( response != 0 ){
		t = 0;
		do {
			memset(buffer, 0, 32);
			if( (err = read(buffer,31)) > 0 ){
				response(response_arg, buffer,err);
			} else {
				t++;
				Timer::wait_msec(1);
			}
		} while( t < timeout);
	}

	return 0;
}

bool Brsp::wait(const char * expected){
	int len;
	int t;
	int ret;
	char c;
	int bytes_recv;
	bytes_recv = 0;
	t = 0;
	len = strlen(expected);
	do {
		ret = read(&c, 1);
		if( ret > 0 ){
			if( expected[bytes_recv] != c ){
				return false;
			}
			bytes_recv++;
			if( bytes_recv == len ){
				return true;
			}
		} else {
			t++;
			Timer::wait_msec(1);
		}
	} while( t < timeout);
	return false;
}

