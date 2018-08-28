#include "Duck.h"

#include <iostream>
#include <assert.h>
#include <memory>  // include this to use std::unique_ptr

namespace designpattern {

Duck::Duck(FlyBehavior* flyBehavior, QuackBehavior* quackBehavior) :
	_flyBehavior(flyBehavior),
	_quackBehavior(quackBehavior) {
		assert(flyBehavior);
		assert(quackBehavior);
	}

Duck::~Duck() {}

void Duck::setFlyBehavior(FlyBehaviorPtr fb) {
	assert(fb);
	_flyBehavior.reset();
	_flyBehavior = std::move(fb);
}
void Duck::setQuackBehavior(QuackBehaviorPtr qb) {
	assert(qb);
	_quackBehavior.reset();
	_quackBehavior = std::move(qb);
}

void Duck::performFly() const {
	_flyBehavior->fly();
}

void Duck::performQuack() const {
	_quackBehavior->quack();
}

void Duck::swim() const
{
    std::cout << "All ducks float, even decoys." << std::endl;
}

}
