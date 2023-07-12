/* this is to control the Battery Fuel Gauge DS2782 (Maxim Integrated) 
 * with MUX/DEMUX module CD74HC4051
 * by Jiwon Kim <kim.j@yonsei.ac.kr>
 */

// DS2782 (FUEL GAUGE) * 4, CD74HC4051 (MUX/DEMUX)  CONTROL
// Used channels are only 0, 1, 2

#include<I2C.h>

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
#define RSGAIN_MSB 0x78
#define RSGAIN_LSB 0x79

#define DS2783_SLAVE_ADDR 0x34

int16_t s_en = 4;
int16_t s0 = 5;
int16_t s1 = 6;
int16_t s2 = 7;

float voltage, current, temperature;
float rsgain; // tmp

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  I2c.begin(); // Start I2C Master
  //I2c.scan();

  pinMode(s_en, OUTPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  
  digitalWrite(s_en, LOW); // Active low
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);
  digitalWrite(s2, HIGH);  
}

void loop() {  
  /*  muxing & demuxing  */
  for(int i = 0; i < 3; i++) {
    readMux(i);
    fetchInfo();
    Serial.print("[");
    Serial.print(i);
    Serial.print("] ");
    Serial.print(voltage);
    Serial.print(", ");
    Serial.print(current);
    Serial.print(", ");
    Serial.println(temperature);    
  }
  Serial.println();
  delay(1000);
}

void fetchInfo() {
  /*  fetch  */
  voltage = fetch_word_11bit(VOLT_MSB);
  current = fetch_word_16bit(CURRENT_MSB);
  temperature = fetch_word_11bit(TEMP_MSB);  

  /*  scaling  */
  voltage = voltage * 4.88 / 1000; // (V)
  temperature = temperature * 0.125;
  current = current * 0.1563; // (mA)
}

int readMux(int channel) {
  int controlPin[] = {s0, s1, s2};
  int muxChannel[8][3] = {
    {0, 0, 0}, // channel 0
    {1, 0, 0}, // channel 1
    {0, 1, 0}, // channel 2
    {1, 1, 0}, // channel 3
    {0, 0, 1}, // channel 4
    {1, 0, 1}, // channel 5
    {0, 1, 1}, // channel 6
    {1, 1, 1} // channel 7    
  };
  for (int i = 0; i < 3; i++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
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

int fetch_word_11bit_point(int command) {
  I2c.read(DS2783_SLAVE_ADDR, command, 2);
  byte b1 = I2c.receive();
  byte b2 = I2c.receive();
  b1 = b1 & 7;
  float value = (b1 << 8) | b2;
  value = value / 1024;
  
  return value;    
}

int fetch_word_16bit(int command) {
  I2c.read(DS2783_SLAVE_ADDR, command, 2);
  byte b1 = I2c.receive();
  byte b2 = I2c.receive();
  int value = (b1 << 8) | b2;
  return value;
}
