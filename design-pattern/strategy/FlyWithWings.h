#ifndef __FLY_WITH_WINGS_H
#define __FLY_WITH_WINGS_H

#include "FlyBehavior.h"
#include <iostream>

namespace designpattern {

class FlyWithWings: public FlyBehavior {
public :
	virtual void fly() override {
		std::cout << "I'm flying!" << std::endl;
	}
};

}
#endif //__FLY_WITH_WINGS_H
