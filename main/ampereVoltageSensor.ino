
void readAmpereVoltage() {
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA() / 1000;
  
  if(busvoltage < 1.5)                      busvoltage = 0;
  if (millis() > (last_samplingAV + 2000))  last_samplingAV = millis();
  
}
