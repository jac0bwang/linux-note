#ifndef DESIGNPATTERN_DECORATOR_HOUSE_BLEND_H_
#define DESIGNPATTERN_DECORATOR_HOUSE_BLEND_H_

#include "beverage.h"

namespace designpattern {
namespace decorator {

class HouseBlend: public Beverage {
 public:
  HouseBlend();
  HouseBlend(Beverage::Size size);

  virtual double cost() const override;
};

} // decorator
} // designpattern

#endif //DESIGNPATTERN_DECORATOR_HOUSE_BLEND_H_
