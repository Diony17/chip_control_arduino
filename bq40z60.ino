/* this is to control the Battery Fuel Gauge BQ40Z60 
 * by Jiwon Kim <kim.j@yonsei.ac.kr>
 */
 
//bq40z60
#include <I2C.h>

#define COMM_TEMPERATURE 0x08
#define COMM_VOLTAGE 0x09
#define COMM_CURRENT 0x0A
#define COMM_RSOC 0x0D
#define COMM_ASOC 0x0E
#define COMM_REMAINING_CAPACITY 0x0F
#define COMM_FULL_CAPACITY 0x10

#define COMM_CELL_VOLTAGE4 0x3C
#define COMM_CELL_VOLTAGE3 0x3D
#define COMM_CELL_VOLTAGE2 0x3E
#define COMM_CELL_VOLTAGE1 0x3F
#define COMM_SOH 0x4F

#define BQ40Z60_SLAVE_ADDR 0x0B
#define TIME_DELTA 1000

void setup() {
  I2c.begin();
  Serial.begin(9600);
  while(!Serial); // wait until serial is ready
  Serial.println("Start");

  I2c.scan();
  I2c.setSpeed(0);  // set speed slow
}

void loop() {
  float temp_c = (float)fetch_word_u(COMM_TEMPERATURE) * 0.1 - 273.16;

  int rsoc = fetch_word_i(COMM_RSOC);
  int asoc = fetch_word_i(COMM_ASOC);
  int volt_mv = fetch_word_u(COMM_VOLTAGE);
  int cell1_volt_mv = fetch_word_u(COMM_CELL_VOLTAGE1);
  int cell2_volt_mv = fetch_word_u(COMM_CELL_VOLTAGE2);
  int cell3_volt_mv = fetch_word_u(COMM_CELL_VOLTAGE3);
  int cell4_volt_mv = fetch_word_u(COMM_CELL_VOLTAGE4);

  int current_ma = fetch_word_i(COMM_CURRENT);

  Serial.print(temp_c);
  Serial.print(" ");
  Serial.print(volt_mv);
  Serial.print(" ");
  Serial.print(cell1_volt_mv);
  Serial.print(" ");
  Serial.print(cell2_volt_mv);
  Serial.print(" ");
  Serial.print(cell3_volt_mv);
  Serial.print(" ");
  Serial.print(cell4_volt_mv);
  Serial.print(" ");
  Serial.print(current_ma);
  Serial.print(" ");
  Serial.print(rsoc);
  Serial.print(" ");
  Serial.println(asoc);
  
  delay(TIME_DELTA);
}

unsigned int fetch_word_u(int command)
{
  I2c.read(BQ40Z60_SLAVE_ADDR, command, 2);
  byte b1 = I2c.receive();
  byte b2 = I2c.receive();
  return (unsigned int)(b1 | (b2 << 8));
}

int fetch_word_i(int command)
{
  I2c.read(BQ40Z60_SLAVE_ADDR, command, 2);
  byte b1 = I2c.receive();
  byte b2 = I2c.receive();
  return (int)(b1 | (b2 << 8));
}


