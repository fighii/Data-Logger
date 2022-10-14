void error(char *str) {
  Serial.print("error: ");
  Serial.println(str);
  while (1);
}

void setting_writeTime() {
  write_time = EEPROM.read(writeTime_addr);
  last_notPressed = millis();
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("Setting");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.setCursor(6, 1);
  lcd.print(write_time);
  lcd.print("s    ");
  while (true) {
    String str = get_button();
    if (str) {
      if (str == "Up") {
        if (millis() - last_notPressed > 20000) {
          write_time += 10;
          delay(100);
        }
        else {
          write_time ++;
          delay(500);
        }
        lcd.setCursor(6, 1);
        lcd.print(write_time);
        lcd.print("s    ");
      }
      if (str == "Down") {

        if (millis() - last_notPressed > 20000) {
          write_time -= 10;
          delay(100);
        }
        else {
          write_time --;
          delay(500);
        }
        if (write_time < 0) {
          write_time = 0;
        }
        lcd.setCursor(6, 1);
        lcd.print(write_time);
        lcd.print("s   ");
      }
      if (str == "Left") {
        lcd.clear();
        EEPROM.write(writeTime_addr, write_time);
        lcd.setCursor(2, 1);
        lcd.print("Save Writing Time:");
        lcd.setCursor(6, 2);
        lcd.print(write_time);
        lcd.print("s");
        delay(3000);
        lcd.clear();
        break;
      }
    }
    if (str == "") {
      last_notPressed = millis();
    }
  }

}

void setting_area() {
  area_addr = sizeof(float);
  MyObject var;
  EEPROM.get( area_addr, var );
  float setting_river_area = var.river_area;
  last_notPressed = millis();

  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("Setting");
  lcd.setCursor(0, 1);
  lcd.print("River: ");
  lcd.setCursor(7, 1);
  lcd.print(setting_river_area);
  lcd.print("m2");

  while (true) {
    String str = get_button();
    if (str) {
      if (str == "Up") {
        if (millis() - last_notPressed > 1000) {
          setting_river_area ++;
          delay(100);
        }
        if (millis() - last_notPressed > 20000) {
          setting_river_area += 10;
          delay(100);
        }
        else {
          setting_river_area += 0.01;
          delay(5);
        }
        lcd.setCursor(7, 1);
        lcd.print(setting_river_area);
        lcd.print("m2   ");

      }
      if (str == "Down") {
        if (millis() - last_notPressed > 1000) {
          setting_river_area --;
          delay(300);
        }
        if (millis() - last_notPressed > 20000) {
          setting_river_area -= 10;
          delay(100);
        }
        else {
          setting_river_area -= 0.01;
          delay(5);
        }
        if (setting_river_area < 0) {
          setting_river_area = 0;
        }
        lcd.setCursor(7, 1);
        lcd.print(setting_river_area);
        lcd.print("m2   ");
      }
      if (str == "Left") {
        lcd.clear();
        EEPROM.put(area_addr, setting_river_area);
        lcd.setCursor(2, 1);
        lcd.print("Save River Area");
        lcd.setCursor(6, 2);
        lcd.print(setting_river_area);
        lcd.print("m2   ");
        delay(3000);
        lcd.clear();
        break;
      }
    }
    if (str == "") {
      last_notPressed = millis();
    }
  }
}

void lcdPrint() {
  if (millis() - last_serial > 2000) { //---------------ready for print to LCD-------------

    if (displayMode == 0) {
      last_serial = millis();
      lcd.setCursor(0, 0);
      lcd.print("F: ");
      lcd.print(flow);
      lcd.print(" m3/s     ");

      lcd.setCursor(0, 1);
      lcd.print("V: ");
      lcd.print(busvoltage);
      lcd.print(" V");

      lcd.setCursor(0, 2);
      lcd.print("I: ");
      lcd.print(abs(current_mA));
      lcd.print(" A");

      lcd.setCursor(0, 3);
      lcd.print("RPM: ");
      lcd.print(rpm);
      lcd.print(" rpm   ");
    }

    if (displayMode == 1) {
      last_serial = millis();
      lcd.setCursor(0, 0);
      lcd.print("Vl: ");
      lcd.print(velocity);
      lcd.print(" m/s     ");

      lcd.setCursor(0, 1);
      lcd.print("V: ");
      lcd.print(busvoltage);
      lcd.print(" V");

      lcd.setCursor(0, 2);
      lcd.print("I: ");
      lcd.print(abs(current_mA));
      lcd.print(" A");

      lcd.setCursor(0, 3);
      lcd.print("RPM: ");
      lcd.print(rpm);
      lcd.print(" rpm   ");
    }
  }
}




String get_button() {
  if (digitalRead(22) == false) {
    return "Up";
  }
  if (digitalRead(24) == false) {
    return "Left";
  }
  if (digitalRead(26) == false) {
    return "Down";
  }
  if (digitalRead(28) == false) {
    return "Right";
  }
  return "";
}
