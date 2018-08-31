#ifndef DESIGNPATTERN_DECORATOR_DARK_ROAST_H_
#define DESIGNPATTERN_DECORATOR_DARK_ROAST_H_

#include "beverage.h"

namespace designpattern {
namespace decorator {

class DarkRoast: public Beverage {
 public:
  DarkRoast();
  DarkRoast(Beverage::Size size);

  virtual double cost() const override;
};

} // decorator
} // designpattern

#endif //DESIGNPATTERN_DECORATOR_DARK_ROAST_H_
