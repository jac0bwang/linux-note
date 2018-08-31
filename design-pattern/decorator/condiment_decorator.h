#ifndef DESIGNPATTERN_DECORATOR_CONDIMNT_DECORATOR_H_
#define DESIGNPATTERN_DECORATOR_CONDIMNT_DECORATOR_H_

#include <string>

#include "beverage.h"

namespace designpattern {
namespace decorator {

class CondimentDecorator : public Beverage {
 public:
  CondimentDecorator(Beverage *beverage);
  ~CondimentDecorator();

  virtual std::string getDescription() const override = 0;
  virtual Size getSize() const override;

 protected:
  Beverage *beverage_;
};

} // decorator
} // designpattern

#endif //DESIGNPATTERN_DECORATOR_CONDIMNT_DECORATOR_H_
