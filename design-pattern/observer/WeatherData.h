#ifndef _WEATHER_DATA_H
#define _WEATHER_DATA_H

#include "Subject.h"

namespace designpattern {
namespace observer {

class Subject;

class WeatherData: public Subject {
private:
    float temperature_;
    float humidity_;
    float pressure_;

public:
    WeatherData();

public:
    void measurementsChanged();
    void setMeasurements(float temperature, float humidity, float pressure);

    float getTemperature() const;
    float getHumidity() const;
    float getPressure() const;

};

} // observer
} // designpattern
#endif //_WEATHER_DATA_H
