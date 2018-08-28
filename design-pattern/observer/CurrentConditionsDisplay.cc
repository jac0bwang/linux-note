#include "CurrentConditionsDisplay.h"
#include "WeatherData.h"

#include <iostream>

namespace designpattern {
namespace observer {

CurrentConditionsDisplay::CurrentConditionsDisplay(Subject& weatherData)
	:temperature_(0.0), humidity_(0.0) {
	weatherData.registerObserver(this);
}

void CurrentConditionsDisplay::update(const Subject& subject) {
	const WeatherData* weatherData;

	if ((weatherData = dynamic_cast<const WeatherData *> (&subject)) != nullptr) {
		temperature_ = weatherData->getTemperature();
		humidity_ = weatherData->getHumidity();
		display();
	}
}
void CurrentConditionsDisplay::display() {
	std::cout << "Current conditions: " << temperature_ << "F degrees and "
		<< humidity_  << "% humidity." << std::endl;
}

} // observer
} // designpattern

