#ifndef _STATISTICS_DISPLAY_H
#define _STATISTICS_DISPLAY_H

#include "IObserver.h"
#include "IDisplayElement.h"

namespace designpattern {
namespace observer {

class Subject;

class StatisticsDisplay: public IObserver, IDisplayElement {
public:
	StatisticsDisplay(Subject& weatherData);

	virtual void update(const Subject& subject) override;
	virtual void display() override;

private:
	float mMaxTemp;
	float mMinTemp;
	float mTempSum;
	int mNumReadings;
};

} // observer
} // designpattern
#endif //_STATISTICS_DISPLAY_H
