/* this is to control the Battery Fuel Gauge STC3100 (STMicroelectronics)
 * by Jiwon Kim <kim.j@yonsei.ac.kr>
 */

// stc3100
#include <Wire.h>

/*  Device address  */
#define STC3100_SLAVE_ADDR  0x70

/*  Command addresses in decimal */
#define REG_MODE  0
#define REG_CTRL  1
#define REG_CHARGE_LOW    2
#define REG_CHARGE_HIGH   3
#define REG_COUNTER_LOW   4
#define REG_COUNTER_HIGH  5
#define REG_CURRENT_LOW   6
#define REG_CURRENT_HIGH  7
#define REG_VOLTAGE_LOW   8
#define REG_VOLTAGE_HIGH  9
#define REG_TEMPERATURE_LOW   10
#define REG_TEMPERATURE_HIGH  11

#define SENSERESISTOR 13 // 이값을 적당히 calibration 해야 함
#define CurrentFactor (4096/SENSERESISTOR) // 4096 = 2^12
#define TIME_DELAY    1000

void STC3100_start() {
  write_byte(REG_MODE, 0x10);
  write_byte(REG_CTRL, 0x02);
}

uint16_t get_value(uint8_t reg) {
    Wire.beginTransmission(STC3100_SLAVE_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(STC3100_SLAVE_ADDR, 2);
    uint8_t low  =  Wire.read();         // print the character
    uint8_t high = Wire.read();

    return ((uint16_t) high << 8) + low;
}

void write_byte(uint8_t location, uint8_t value) {
    Wire.beginTransmission(STC3100_SLAVE_ADDR);
    Wire.write(location); // 이 주소값에
    Wire.write(value);    // 이 값을 써주세요.
    Wire.endTransmission();
}

void setup() {
  Serial.begin(9600);  
  Wire.begin();
  STC3100_start();  // chip을 껐다가 킬때마다 필요
}

void loop() {
  uint16_t voltage_i     = get_value(REG_VOLTAGE_LOW);
  uint16_t temperature_i = get_value(REG_TEMPERATURE_LOW);
  uint16_t current_i     = get_value(REG_CURRENT_LOW);
  float voltage, temperature, current;
  long current_uA;
  bool tSign = false, cSign = false;

  // 2's complement
  if(temperature_i > 0x2000) {
    temperature_i = (~temperature_i) + 1;
    tSign = true;
  }
  if(current_i > 0x2000) {
    current_i = (~current_i) + 1; 
    cSign = true;
  }
  
  current_i &= 0x3fff; // mask unused bits

  // Current processing
  current_uA = ((long)current_i * CurrentFactor) >> 12;
  
  
  temperature = temperature_i * 0.125;  
  voltage = voltage_i * 2.44;
  current = current_uA * 11.77;
  
  if(tSign == true) temperature *= -1;
  if(cSign == true) current *= -1;

  unsigned long cTime;
  cTime = millis();

  Serial.print(cTime);   Serial.print("\t");
  Serial.print(voltage); Serial.print("\t");
  Serial.print(current); Serial.print("\t");
  Serial.println(temperature);
  
  delay(TIME_DELAY/10);
}