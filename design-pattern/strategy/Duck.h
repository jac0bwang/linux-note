#ifndef __DUCK_H
#define __DUCK_H

#include "FlyBehavior.h"
#include "QuackBehavior.h"

#include <iostream>
#include <assert.h>
#include <memory>  // include this to use std::unique_ptr

namespace designpattern {

class Duck {
protected:
	Duck(FlyBehavior* flyBehavior, QuackBehavior* quackBehavior);

public :
	using FlyBehaviorPtr = std::unique_ptr<FlyBehavior>;
	using QuackBehaviorPtr = std::unique_ptr<QuackBehavior>;

	virtual ~Duck() = 0;

	void setFlyBehavior(FlyBehaviorPtr fb);
	void setQuackBehavior(QuackBehaviorPtr qb);

	void performFly() const;
	void performQuack() const;

	void swim() const;

	virtual void display() const = 0;

private :
	FlyBehaviorPtr _flyBehavior;
	QuackBehaviorPtr _quackBehavior;

	Duck(const Duck&); // Disable copy constructor
	void operator=(const Duck&); // Disable assignment operator
};

} // designpattern
#endif //__DUCK_H
