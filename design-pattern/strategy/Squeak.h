#ifndef __SQUEAK_H
#define __SQUEAK_H

#include "QuackBehavior.h"
#include <iostream>

namespace designpattern {

class Squeak: public QuackBehavior {
public :
	virtual void quack() override {
		std::cout << "<< Squeak >>" << std::endl;
	}
};

}

#endif //__SQUEAK_H
