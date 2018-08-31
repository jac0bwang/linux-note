#ifndef DESIGNPATTERN_DECORATOR_MOCHA_H_
#define DESIGNPATTERN_DECORATOR_MOCHA_H_

#include "condiment_decorator.h"

namespace designpattern {
namespace decorator {

class Mocha: public CondimentDecorator {
 public:
  Mocha(Beverage *beverage);

  virtual std::string getDescription() const override;
  virtual double cost() const override;
};

} // decorator
} // designpattern

#endif //DESIGNPATTERN_DECORATOR_MOCHA_H_
