#include "whip.h"

namespace designpattern {
namespace decorator {

Whip::Whip(Beverage *beverage) : CondimentDecorator(beverage)
{}

std::string Whip::getDescription() const
{
  return beverage_->getDescription() + ", Whip";
}


double Whip::cost() const
{
  return 0.10 + beverage_->cost();
}

} // decorator
} // designpattern
