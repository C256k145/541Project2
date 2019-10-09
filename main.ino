int out_pin = 5;
float output = 11;
char serial_data = 0;

float findDutyCycle(float desired_voltage) {
//  somefunc(desired_voltage);
  return(0.5);
}

void writeDutyCycle(float duty_cycle) {
  if(duty_cycle > 0.25 && duty_cycle < 0.75) {
    int write_val = round(duty_cycle*255);
    analogWrite(out_pin, write_val);
  }
}

// Source: https://playground.arduino.cc/Code/PwmFrequency/
void set_pwm(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}



void setup() {
  pinMode(out_pin, OUTPUT);
  Serial.begin(9600);
  set_pwm(5,1);
}

void loop() {
  if(Serial.available() > 0) {
    serial_data = Serial.read();
    delay(25);

    switch(serial_data) {
      case 'd':
        if(output < 22) {
          output += 0.5;
        }
        break;
      case 'a':
        if(output > 11) {
          output -= 0.5;
        }
        break;
    }

    writeDutyCycle(findDutyCycle(output));
    Serial.print("Desired Load voltage: ");
    Serial.print(output); Serial.println(" V");
    // measure_voltage();
    Serial.println();
  }
}