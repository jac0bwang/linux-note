
#include "MiniDuckSimulator.h"

using namespace designpattern;

void testMallardDuck()
{
	// default mallard duck
	std::unique_ptr<Duck> mallard(new MallardDuck());
	mallard->display();
	mallard->performQuack();
	mallard->performFly();

	// changing behaviors
	mallard->setFlyBehavior(std::unique_ptr<FlyBehavior>(new FlyNoWay()));
	mallard->setQuackBehavior(std::unique_ptr<QuackBehavior>(new MuteQuack()));

	// runtime modified mallard duck
	mallard->performFly();
	mallard->performQuack();
}

void testModelDuck()
{
	std::unique_ptr<Duck> model(new ModelDuck());
	model->performFly();
	model->setFlyBehavior(std::unique_ptr<FlyBehavior>(new FlyRocketPowered()));
	model->performFly();
}

int main() {

	std::cout << "Mallard duck test:" << std::endl;
	testMallardDuck();

	std::cout << "\nModel duck test:" << std::endl;
	testModelDuck();
	return 0;
}
