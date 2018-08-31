#include "condiment_decorator.h"

namespace designpattern {
namespace decorator {

CondimentDecorator::CondimentDecorator(Beverage *beverage)
    : Beverage(), beverage_(beverage) {}

CondimentDecorator::~CondimentDecorator() {
  delete beverage_;
}

CondimentDecorator::Size CondimentDecorator::getSize() const {
  return beverage_->getSize();
}

} // decorator
} // designpattern
