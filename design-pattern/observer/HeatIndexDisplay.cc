#include "HeatIndexDisplay.h"

#include <iostream>

#include "WeatherData.h"

namespace designpattern {
namespace observer {

HeatIndexDisplay::HeatIndexDisplay(Subject& weatherData)
	:mHeatIndex(0.0) {
	weatherData.registerObserver(this);
}

void HeatIndexDisplay::update(const Subject& subject) {
	const WeatherData* weatherData;

	if ((weatherData = dynamic_cast<const WeatherData *> (&subject)) != nullptr) {
		mHeatIndex = computeHeatIndex(weatherData->getTemperature(),
					      weatherData->getHumidity());
		display();
	}
}

float HeatIndexDisplay::computeHeatIndex(float t, float rh) {
	return (float)((16.923 + (0.185212 * t) + (5.37941 * rh) - (0.100254 * t * rh)
			+ (0.00941695 * (t * t)) + (0.00728898 * (rh * rh))
			+ (0.000345372 * (t * t * rh)) - (0.000814971 * (t * rh * rh)) +
			(0.0000102102 * (t * t * rh * rh)) - (0.000038646 * (t * t * t)) + (0.0000291583 *
											    (rh * rh * rh)) + (0.00000142721 * (t * t * t * rh)) +
			(0.000000197483 * (t * rh * rh * rh)) - (0.0000000218429 * (t * t * t * rh * rh)) +
			0.000000000843296 * (t * t * rh * rh * rh)) -
		       (0.0000000000481975 * (t * t * t * rh * rh * rh)));

}

void HeatIndexDisplay::display() {
	std::cout << "Heat index is " << mHeatIndex << std::endl;
}

} // observer
} // designpattern

