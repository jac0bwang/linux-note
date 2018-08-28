#ifndef __MALLARD_DUCK_H
#define __MALLARD_DUCK_H

#include "MiniDuckSimulator.h"

#include <iostream>
#include <assert.h>
#include <memory>  // include this to use std::unique_ptr

namespace designpattern {

class MallardDuck : public Duck {
public:
	MallardDuck() : Duck( new FlyWithWings(), new Quack()  ) {

	}
	void display() const {
		std::cout << "I'm a real Mallard duck" << std::endl;
	}
};

} // designpattern
#endif //__MALLARD_DUCK_H
