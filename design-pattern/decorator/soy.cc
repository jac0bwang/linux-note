#include "soy.h"

namespace designpattern {
namespace decorator {

Soy::Soy(Beverage *beverage) : CondimentDecorator(beverage)
{}

std::string Soy::getDescription() const
{
  return beverage_->getDescription() + ", Soy";
}


double Soy::cost() const
{
  double base_cost = beverage_->cost();
  switch (beverage_->getSize()) {
    case Beverage::Size::SMALL:
      base_cost += 0.1;
      break;
    case Beverage::Size::MEDIUM:
      base_cost += 0.15;
      break;
    case Beverage::Size::BIG:
      base_cost += 0.20;
      break;
  }
  return base_cost;
}

} // decorator
} // designpattern
