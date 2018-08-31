#ifndef DESIGNPATTERN_DECORATOR_WHIP_H_
#define DESIGNPATTERN_DECORATOR_WHIP_H_

#include "condiment_decorator.h"

namespace designpattern {
namespace decorator {

class Whip: public CondimentDecorator {
 public:
  Whip(Beverage *beverage);

  virtual std::string getDescription() const override;
  virtual double cost() const override;
};

} // decorator
} // designpattern

#endif //DESIGNPATTERN_DECORATOR_WHIP_H_
