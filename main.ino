#include "Arduino.h"

// int out_pin = 5;
int duty_cycle_pin = 5;
// int voltage_adjust_pin = 3;
int input_pin = A7;
char serial_data = 0;
float duty_cycle = 0.5;

float output = 11;
float max_voltage = 17.0;
float min_voltage = 12.0;
float arduino_5V = 4.88;

float R_small = 1000000;
float R_big = 3000000;

// float R1 = 0;
// float R2 = 0;
// float gain = (R1 + R2)/(R2);
// float v_ref = 7.0;

void writeDutyCycle(float value) {
  int write_val = round(value*255);
  analogWrite(duty_cycle_pin, write_val);
}

float measureVoltage() {
  float sensor = analogRead(input_pin);
  float measured_voltage = input_pin*(255/arduino_5V);
  float v_load = ((R_big+R_small)/R_small)*measured_voltage;
  return(v_load);
}

float adjustDutyCycle(float v_load) {
  if(v_load < output) {
    if(duty_cycle == 0) {
      Serial.println("Minimum Duty Cycle Reached");
    }
    else {
      duty_cycle -= 0.01;
    }
  }
  else {
    if(duty_cycle > 0.75) {
      Serial.println("Maximum Duty Cycle Reached");
    }
    else {
      duty_cycle += 0.01;
    }
  }
}

// Source: https://playground.arduino.cc/Code/PwmFrequency/
// This function allows us to adjust the PWM frequency to one of several values.
void setPwmFrequency(int pin, int divisor) {
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
  pinMode(duty_cycle_pin, OUTPUT);
  // pinMode(voltage_adjust_pin, OUTPUT);
  pinMode(input_pin, INPUT);
  Serial.begin(9600);
  setPwmFrequency(5,1); // Sets pin 5 to 62.5 KHz
}

void loop() {
  if(Serial.available() > 0) {
    serial_data = Serial.read();
    delay(25);

    switch(serial_data) {
      case 'd':
        if(output < max_voltage) {
          output += 0.5;
        }
        break;
      case 'a':
        if(output > min_voltage) {
          output -= 0.5;
        }
        break;
    }

    adjustDutyCycle(measureVoltage());
  }
  writeDutyCycle(duty_cycle);
}