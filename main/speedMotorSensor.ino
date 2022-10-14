void speedCount(){
  tick++;
}

void readSpeed() {
  if (tick >= 1) {
    rpm = 60000 / (millis() - last_samplingSpeed);
    last_samplingSpeed = millis();
    tick = 0;
  }

  if ((millis() - last_samplingSpeed) > 6000) {
    last_samplingSpeed = millis();
    rpm = 0;
  }
  
  if (last_rpm != rpm) {
    last_rpm = rpm;
  }
}
