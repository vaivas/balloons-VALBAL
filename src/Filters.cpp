/*
  Stanford Student Space Initiative
  Balloons | VALBAL | February 2017
  Davy Ragland | dragland@stanford.edu
  Michal Adamkiewicz | mikadam@stanford.edu

  File: Filters.cpp
  --------------------------
  Implementation of Filters.h
*/

#include "Filters.h"

/**********************************  SETUP  ***********************************/
/*
  function: init
  ---------------------------------
  This function initializes the filter objects
*/
bool Filters::init() {
  bool sucess = true;
  return sucess;
}

/********************************  FUNCTIONS  *********************************/
/*
  function: enableSensors
  ---------------------------------
  This function selectivly enables and disables the
  sensors included in fused calculations.
*/
void Filters::enableSensors(bool BMP1Enable, bool BMP2Enable, bool BMP3Enable, bool BMP4Enable) {
  enabledSensors[0] = BMP1Enable;
  enabledSensors[1] = BMP2Enable;
  enabledSensors[2] = BMP3Enable;
  enabledSensors[3] = BMP4Enable;
  numSensors = 0;
  for (size_t i = 0; i < 4; i++) if (enabledSensors[i]) numSensors++;
}

/*
  function: getTemp
  ---------------------------------
  This function returns a sensor fused temperature.
*/
double Filters::getTemp(double RAW_TEMP_1, double RAW_TEMP_2, double RAW_TEMP_3, double RAW_TEMP_4) {
  double temp = 0;
  if (enabledSensors[0]) temp += RAW_TEMP_1;
  if (enabledSensors[1]) temp += RAW_TEMP_2;
  if (enabledSensors[2]) temp += RAW_TEMP_3;
  if (enabledSensors[3]) temp += RAW_TEMP_4;
  return temp / numSensors;
}

/*
  function: getPressure
  ---------------------------------
  This function returns a sensor fused reading.
*/
double Filters::getPressure(double RAW_PRESSURE_1, double RAW_PRESSURE_2, double RAW_PRESSURE_3, double RAW_PRESSURE_4) {
  double press = 0;
  if (enabledSensors[0]) press += RAW_PRESSURE_1;
  if (enabledSensors[1]) press += RAW_PRESSURE_2;
  if (enabledSensors[2]) press += RAW_PRESSURE_3;
  if (enabledSensors[3]) press += RAW_PRESSURE_4;
  return press / numSensors;
}

/*
  function: getCalculatedAltitude
  ---------------------------------
  This function returns a higher precision altitude value
  based on the US 1976 Standard Atmosphere.
*/
float Filters::getCalculatedAltitude(float pressure, float pressureBaseline) {
  if (pressure > 22632.1) return (44330.7 * (1 - pow(pressure / pressureBaseline, 0.190266)));
  return -6341.73 * log((0.176481 * pressure) / 22632.1);
}

/*
  function: getAltitude
  ---------------------------------
  This function returns a sensor fused reading.
*/
double Filters::getAltitude(double RAW_ALTITUDE_1, double RAW_ALTITUDE_2, double RAW_ALTITUDE_3, double RAW_ALTITUDE_4) {
  altitudeLast = altitudeCurr;
  double alt = 0;
  if (enabledSensors[0]) alt += RAW_ALTITUDE_1;
  if (enabledSensors[1]) alt += RAW_ALTITUDE_2;
  if (enabledSensors[2]) alt += RAW_ALTITUDE_3;
  if (enabledSensors[3]) alt += RAW_ALTITUDE_4;
  altitudeCurr =  alt / numSensors;
  //TODO Kalman FIltering here
  ASCENT_RATE_BUFFER[ascentRateIndex] = (altitudeCurr - altitudeLast) / ((millis() - ascentRateLast) / 1000.0);
  ascentRateLast = millis();
  ascentRateIndex++;
  ascentRateIndex %= BUFFER_SIZE;
  return altitudeCurr;
}

/*
  function: getAscentRate
  ---------------------------------
  This function returns the current ascent rate.
*/
double Filters::getAscentRate() {
  float ascentRateTotal = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) ascentRateTotal += ASCENT_RATE_BUFFER[i];
  return  ascentRateTotal / BUFFER_SIZE;
}
