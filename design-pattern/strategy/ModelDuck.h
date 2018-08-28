#ifndef __MODEL_DUCK_H
#define __MODEL_DUCK_H

#include "MiniDuckSimulator.h"

#include <iostream>
#include <assert.h>
#include <memory>  // include this to use std::unique_ptr

namespace designpattern {

class ModelDuck : public Duck {
public:
	ModelDuck() : Duck( new FlyNoWay(), new Quack()  ) {

	}
	void display() const {
		std::cout << "I'm a Model duck" << std::endl;
	}
};

} // designpattern
#endif //__MODEL_DUCK_H
