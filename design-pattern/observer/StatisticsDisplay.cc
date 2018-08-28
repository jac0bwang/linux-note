#include "StatisticsDisplay.h"

#include <iostream>
#include <limits>

#include "WeatherData.h"

namespace designpattern {
namespace observer {

StatisticsDisplay::StatisticsDisplay(Subject& weatherData)
	:mMaxTemp(std::numeric_limits<float>::min()),
	mMinTemp(std::numeric_limits<float>::min()),
	mTempSum(0.f), mNumReadings(0) {
	weatherData.registerObserver(this);
}

void StatisticsDisplay::update(const Subject& subject) {
	const WeatherData* weatherData;

	if ((weatherData = dynamic_cast<const WeatherData *> (&subject)) != nullptr) {
		mTempSum += weatherData->getTemperature();
		mNumReadings++;

		if (weatherData->getTemperature() > mMaxTemp)
			mMaxTemp = weatherData->getTemperature();

		if (weatherData->getTemperature() < mMinTemp)
			mMinTemp = weatherData->getTemperature();

		display();
	}
}

void StatisticsDisplay::display() {
	std::cout << "Avg: " << mTempSum / mNumReadings << "F - Max: "
		<< mMaxTemp << "F - Min: " << mMinTemp << "F." << std::endl;

}

} // observer
} // designpattern

