#ifndef __FLY_ROCKET_POWERED_H
#define __FLY_ROCKET_POWERED_H

#include "FlyBehavior.h"
#include <iostream>

namespace designpattern {

class FlyRocketPowered : public FlyBehavior {
public :
	virtual void fly() override {
		std::cout << "I'm flying with a rocket!" << std::endl;
	}
};

}
#endif //__FLY_ROCKET_POWERED_H
