#ifndef _CURRENT_CONDITIONS_DISPLAY_H
#define _CURRENT_CONDITIONS_DISPLAY_H

#include "IObserver.h"
#include "IDisplayElement.h"

namespace designpattern {
namespace observer {

class Subject;

class CurrentConditionsDisplay : public IObserver, IDisplayElement {
public:
	CurrentConditionsDisplay(Subject& weatherData);

	virtual void update(const Subject& subject) override;
	virtual void display() override;

private:
	float temperature_;
	float humidity_;
};

} // observer
} // designpattern
#endif //_CURRENT_CONDITIONS_DISPLAY_H
