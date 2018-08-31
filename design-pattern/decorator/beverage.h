#ifndef DESIGNPATTERN_DECORATOR_BEVERAGE_H_
#define DESIGNPATTERN_DECORATOR_BEVERAGE_H_

#include <string>

namespace designpattern {
namespace decorator {

class Beverage {
 public:
  enum class Size {
    SMALL,
    MEDIUM,
    BIG
  };

  virtual ~Beverage();
  virtual std::string getDescription() const;
  virtual double cost() const = 0;
  virtual Size getSize() const;
  void setSize(Size size);


protected:
  Beverage();
  Beverage(const std::string& description, Size size);

protected:
  std::string description_;
  Size size_;

};

} // decorator
} // designpattern

#endif //DESIGNPATTERN_DECORATOR_BEVERAGE_H_
