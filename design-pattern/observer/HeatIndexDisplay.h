#ifndef _HEATINDEX_DISPLAY_H
#define _HEATINDEX_DISPLAY_H

#include "IObserver.h"
#include "IDisplayElement.h"

namespace designpattern {
namespace observer {

class Subject;

class HeatIndexDisplay: public IObserver, IDisplayElement {
public:
	HeatIndexDisplay(Subject& weatherData);

	virtual void update(const Subject& subject) override;
	virtual void display() override;

private:
	float computeHeatIndex(float t, float rh);

	float mHeatIndex;
};

} // observer
} // designpattern
#endif //_HEATINDEX_DISPLAY_H
