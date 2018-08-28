#ifndef __RUBBER_DUCK_H
#define __RUBBER_DUCK_H

#include "MiniDuckSimulator.h"

#include <iostream>
#include <assert.h>
#include <memory>  // include this to use std::unique_ptr

namespace designpattern {

class RubberDuck : public Duck {
public:
	RubberDuck() : Duck( new FlyNoWay(), new Squeak()  ) {
	}
	void display() const {
		std::cout << "I'm a Rubber duck" << std::endl;
	}
};

} // designpattern
#endif //__RUBBER_DUCK_H
