#include "Subject.h"

#include <algorithm>

namespace designpattern {
namespace observer {

Subject::Subject()
	: observers_()
	, changed_(false) {
}

Subject::~Subject() {}

void Subject::registerObserver(IObserver* observer) {

	observers_.push_back(observer);
}

void Subject::removeObserver(IObserver* observer) {
	observers_.erase(std::find(observers_.begin(), observers_.end(), observer));
}

void Subject::notifyObservers() {
	if (changed_) {
		for (auto observerPtr: observers_)
			observerPtr->update(*this);
	}
	changed_ = false;
}
void Subject::setChanged() {
	changed_ = true;
}
void Subject::clearChanged() {
	changed_ = false;
}

bool Subject::hasChanged() const {
	return changed_;
}

} // observer
} // designpattern
