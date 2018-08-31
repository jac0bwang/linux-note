#include "espresso.h"

namespace designpattern {
namespace decorator {

Espresso::Espresso() : Espresso(Beverage::Size::MEDIUM) {}

Espresso::Espresso(Beverage::Size size) : Beverage("Espresso", size) {
}

double Espresso::cost() const {
  return 1.99;
}

} // decorator
} // designpattern
