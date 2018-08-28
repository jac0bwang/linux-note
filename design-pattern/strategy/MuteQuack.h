#ifndef __MUTE_QUACK_BEHAVIOR_H
#define __MUTE_QUACK_BEHAVIOR_H

#include "QuackBehavior.h"
#include <iostream>

namespace designpattern {

class MuteQuack : public QuackBehavior {
public :
	virtual void quack() override {
		std::cout << "<< Silence >>" << std::endl;
	}
};

}

#endif //__MUTE_QUACK_BEHAVIOR_H
