void pulseCounter() {
  pulseCount++;
}

void readFlow() {
  if ((millis() - last_samplingFR) > 1000) {
    detachInterrupt(digitalPinToInterrupt(flowPin));
    area_addr = sizeof(float);
    MyObject var;
    EEPROM.get( area_addr, var );
    flowRate = ((1000.0 / (millis() - last_samplingFR)) * pulseCount) / calibrationFactor;
    last_samplingFR = millis();
    flow = flowRate / 60 * (var.river_area / 0.025);
    velocity = cbrt(flow);
    pulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(flowPin), pulseCounter, FALLING);
  }
}
