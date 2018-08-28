#ifndef __DECOY_DUCK_H
#define __DECOY_DUCK_H

#include "MiniDuckSimulator.h"

#include <iostream>
#include <assert.h>
#include <memory>  // include this to use std::unique_ptr

namespace designpattern {

class DecoyDuck : public Duck {
public:
	DecoyDuck() : Duck( new FlyNoWay(), new MuteQuack()  ) {

	}
	void display() const {
		std::cout << "I'm a Decoy duck" << std::endl;
	}
};

} // designpattern
#endif //__DECOY_DUCK_H
