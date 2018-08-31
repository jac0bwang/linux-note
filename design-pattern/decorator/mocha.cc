#include "mocha.h"

namespace designpattern {
namespace decorator {

Mocha::Mocha(Beverage *beverage) : CondimentDecorator(beverage)
{}

std::string Mocha::getDescription() const
{
  return beverage_->getDescription() + ", Mocha";
}


double Mocha::cost() const
{
  return 0.20 + beverage_->cost();
}

} // decorator
} // designpattern
