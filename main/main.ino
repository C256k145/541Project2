#include "Arduino.h"

int duty_cycle_pin = 5;
int input_pin = A4;
char serial_data = 0;
float duty_cycle = 0.0;
int arr_index = 0;

float output = 12;
float max_voltage = 17.0;
float min_voltage = 12.0;
float arduino_5V = 4.97;
float actual = 0;

float measured_voltage = 0;

float R_small = 1.01;
float R_big = 2.693;

void writeDutyCycle(float value) {
  value = (1-value);
  int write_val = round(value*255);
  analogWrite(duty_cycle_pin, write_val);
}

float measureVoltage() {
  float v_load = 0;
  for(int i = 0;i < 100; i++) {
    float sensor = analogRead(input_pin);
    measured_voltage = sensor*(arduino_5V/1024.0); 
    v_load += ((R_big+R_small)/R_small)*measured_voltage;
  }
  return(v_load/100);
}

float adjustDutyCycle(float v_load) {
  if(v_load > output) {
    if(duty_cycle<=0)
      duty_cycle += 0.001;
    else
      duty_cycle -= 0.001;
  }
  else if(v_load < output) {
    if(duty_cycle>=0.6)
      duty_cycle -= 0.001;
    else
      duty_cycle += 0.001;
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
  pinMode(input_pin, INPUT);
  Serial.begin(9600);
  setPwmFrequency(duty_cycle_pin, 1); // Sets pwm output to 62.5 KHz
}

void loop() {
//  delay(1);
// The arduino was consistently reading the voltage lower than it should,
// so 'fudge' was found as an average of the actual/measured values from 12-17
  float fudge =1.07595;                  
  actual = measureVoltage()*fudge;
  adjustDutyCycle(actual);
  writeDutyCycle(duty_cycle);
  if(Serial.available() > 0) {
    serial_data = Serial.read();

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
    Serial.print("Actual: ");
    Serial.println(actual);
    Serial.print("Desired Voltage: ");
    Serial.println(output);
    Serial.print("Duty Cycle: ");
    Serial.println(duty_cycle);
    Serial.println();
  }
}
