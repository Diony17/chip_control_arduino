/* this is to control the Battery Fuel Gauge DS2782 (Maxim Integrated)
 * by Jiwon Kim <kim.j@yonsei.ac.kr>
 */

// DS2782
#define STATUS 01
#define RAAC_MSB 0x02
#define RAAC_LSB 0x03
#define RSAC_MSB 0x04
#define RSAC_LSB 0x05
#define RARC 06
#define RSRC 07
#define IAVG_MSB 0x08
#define IAVG_LSB 0x09
#define TEMP_MSB 0x0A
#define TEMP_LSB 0x0B
#define VOLT_MSB 0x0C
#define VOLT_LSB 0x0D
#define CURRENT_MSB 0x0E
#define CURRENT_LSB 0x0F

#define DS2783_SLAVE_ADDR 0x34

#include<I2C.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  I2c.begin(); // Start I2C Master
  I2c.scan();
}

void loop() {  
  float voltage, current, temperature;
  /*  fetch  */
  voltage = fetch_word_11bit(VOLT_MSB);
  current = fetch_word_16bit(CURRENT_MSB);
  temperature = fetch_word_11bit(TEMP_MSB);

  /*  scaling  */
  voltage = voltage * 4.88 / 1000;
  temperature = temperature * 0.125;

  Serial.print("Voltage: ");
  Serial.println(voltage);
  Serial.print("Current: ");
  Serial.println(current);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  Serial.println();
  delay(1000);
}

int fetch_word_11bit(int command) {
  I2c.read(DS2783_SLAVE_ADDR, command, 2);
  byte b1 = I2c.receive();
  byte b2 = I2c.receive();
  int value = (b1 << 3) | (b2 >> 5);
  if(value > (1 << 10)) {    
    return (value ^ (1 << 10)) * -1;
  }
  else {
    return value;    
  }  
}


int fetch_word_16bit(int command) {
  I2c.read(DS2783_SLAVE_ADDR, command, 2);
  byte b1 = I2c.receive();
  byte b2 = I2c.receive();
  int value = (b1 << 8) | b2;
  return value;    
}