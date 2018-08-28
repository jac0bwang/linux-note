#ifndef __FLY_NOWAY_H
#define __FLY_NOWAY_H

#include "FlyBehavior.h"
#include <iostream>

namespace designpattern {

class FlyNoWay : public FlyBehavior
{
public :
	virtual void fly() override {
		std::cout << "I cann't fly :(" << std::endl;
	}
};

}
#endif //__FLY_NOWAY_H
