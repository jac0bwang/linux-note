#ifndef __RED_HEAD_DUCK_H
#define __RED_HEAD_DUCK_H

#include "MiniDuckSimulator.h"

#include <iostream>
#include <assert.h>
#include <memory>  // include this to use std::unique_ptr

namespace designpattern {

class RedHeadDuck : public Duck {
public:
	RedHeadDuck() : Duck( new FlyWithWings(), new Quack()  ) {

	}
	void display() const {
		std::cout << "I'm a Red Head duck" << std::endl;
	}
};

} // designpattern
#endif //__RED_HEAD_DUCK_H
