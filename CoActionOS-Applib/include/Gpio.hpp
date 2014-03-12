
#ifndef APPLIB_GPIO_HPP_
#define APPLIB_GPIO_HPP_

#include <dev/gpio.h>
#include "Pchar.hpp"

class Gpio : public Periph {
public:
	Gpio(port_t port);
	static int getports(){ return HWPL_GPIO_PORTS; }
	int open(int flags = 0);
	int getattr(gpio_attr_t * attr);
	int setattr(gpio_attr_t * attr);
	int setmask(unsigned int mask);
	int clrmask(unsigned int mask);
	unsigned int get(void);
	int set(unsigned int value);

#ifdef __HWPL_ONLY__
	int close();
#endif

private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_GPIO_PORTS];
	static bool isinitialized;
#endif
};

#endif /* APPLIB_GPIO_HPP_ */
