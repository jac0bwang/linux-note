#ifndef DESIGNPATTERN_DECORATOR_DECAF_H_
#define DESIGNPATTERN_DECORATOR_DECAF_H_

#include "beverage.h"

namespace designpattern {
namespace decorator {

class Decaf: public Beverage {
 public:
  Decaf();
  Decaf(Beverage::Size size);

  virtual double cost() const override;
};

} // decorator
} // designpattern

#endif //DESIGNPATTERN_DECORATOR_DECAF_H_
