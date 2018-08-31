#ifndef DESIGNPATTERN_DECORATOR_ESPRESSO_H_
#define DESIGNPATTERN_DECORATOR_ESPRESSO_H_

#include "beverage.h"

namespace designpattern {
namespace decorator {

class Espresso: public Beverage {
 public:
  Espresso();
  Espresso(Beverage::Size size);

  virtual double cost() const override;
};

} // decorator
} // designpattern

#endif //DESIGNPATTERN_DECORATOR_ESPRESSO_H_
