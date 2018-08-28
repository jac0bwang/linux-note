#include "WeatherData.h"

#include "CurrentConditionsDisplay.h"
#include "ForecastDisplay.h"
#include "StatisticsDisplay.h"

// uncomment the following #define to include Heat Index
#define _HEAT_INDEX_DISPLAY_

#ifdef _HEAT_INDEX_DISPLAY_
#include "HeatIndexDisplay.h"
#endif

using namespace designpattern::observer;

int main(int argc, char* argv[]) {
	WeatherData weatherData;

	CurrentConditionsDisplay currentDisplay(weatherData);
	StatisticsDisplay statisticsDisplay(weatherData);
	ForecastDisplay forecastDisplay(weatherData);

#ifdef _HEAT_INDEX_DISPLAY_
	HeatIndexDisplay heatIndexDisplay(weatherData);
#endif
	weatherData.setMeasurements(80, 65, 30.4f);
	weatherData.setMeasurements(82, 70, 29.2f);
	weatherData.setMeasurements(78, 90, 29.2f);
	return 0;
}
