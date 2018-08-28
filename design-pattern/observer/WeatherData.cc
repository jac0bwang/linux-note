#include "WeatherData.h"

namespace designpattern {
namespace observer {

WeatherData::WeatherData()
	: Subject() ,temperature_(0.0), humidity_(0.0), pressure_(0.0)
{}

void WeatherData::measurementsChanged() {
	setChanged();
	notifyObservers();
}

void WeatherData::setMeasurements(float temperature, float humidity, float pressure) {
	temperature_ = temperature;
	humidity_ = humidity;
	pressure_ = pressure;
	measurementsChanged();
}

float WeatherData::getTemperature() const {
	return temperature_;
}

float WeatherData::getHumidity() const {
	return humidity_;
}

float WeatherData::getPressure() const {
	return pressure_;
}

} // observer
} // designpattern

