#include "dark_roast.h"

namespace designpattern {
namespace decorator {

DarkRoast::DarkRoast() : DarkRoast(Beverage::Size::MEDIUM) {}

DarkRoast::DarkRoast(Beverage::Size size) : Beverage("Dark Roast", size) {
}

double DarkRoast::cost() const {
  return 0.99;
}

} // decorator
} // designpattern
