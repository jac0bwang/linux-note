#include "beverage.h"

namespace designpattern {
namespace decorator {

Beverage::Beverage() : description_("Unknown Beverage"), size_(Size::MEDIUM) {}

Beverage::Beverage(const std::string& description, Size size)
                   : description_(description), size_(size)
{}

Beverage::~Beverage() {}

std::string Beverage::getDescription() const {
  return description_;
}

Beverage::Size Beverage::getSize() const {
  return size_;
}

void Beverage::setSize(Beverage::Size size) {
  size_ = size;
}

} // decorator
} // designpattern
