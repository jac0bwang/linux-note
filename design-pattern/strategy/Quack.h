#ifndef __QUACK_H
#define __QUACK_H

#include "QuackBehavior.h"
#include <iostream>

namespace designpattern {

class Quack : public QuackBehavior {
public :
	virtual void quack() override {
		std::cout << "<< quack >>" << std::endl;
	}
};

}

#endif //__QUACK_H
