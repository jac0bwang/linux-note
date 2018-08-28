#ifndef __FAKE_QUACK_BEHAVIOR_H
#define __FAKE_QUACK_BEHAVIOR_H

#include "QuackBehavior.h"
#include <iostream>

namespace designpattern {

class FakeQuack : public QuackBehavior {
public :
	virtual void quack() override {
		std::cout << "<< Fake Qakw >>" << std::endl;
	}
};

}

#endif //__FAKE_QUACK_BEHAVIOR_H
