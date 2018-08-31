#include "house_blend.h"

namespace designpattern {
namespace decorator {

HouseBlend::HouseBlend() : HouseBlend(Beverage::Size::MEDIUM) {}

HouseBlend::HouseBlend(Beverage::Size size) : Beverage("HouseBlend", size) {
}

double HouseBlend::cost() const {
  return 1.99;
}

} // decorator
} // designpattern
