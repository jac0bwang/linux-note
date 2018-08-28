#ifndef _FORECAST_DISPLAY_H
#define _FORECAST_DISPLAY_H

#include "IObserver.h"
#include "IDisplayElement.h"

namespace designpattern {
namespace observer {

class Subject;

class ForecastDisplay : public IObserver, IDisplayElement {
public:
	ForecastDisplay(Subject& weatherData);

	virtual void update(const Subject& subject) override;
	virtual void display() override;

private:
	float mCurrentPressure;
	float mPreviousPressure;
};

} // observer
} // designpattern
#endif //_FORECAST_DISPLAY_H
