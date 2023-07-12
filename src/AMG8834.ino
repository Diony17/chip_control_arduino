/* this is to control the Infrared Array (Grid EYE 8x8) Sensor AMG8834 (Panasonic)
 * by Jiwon Kim <kim.j@yonsei.ac.kr>
 */
 
// AMG8834

#include<I2C.h>

uint16_t value[64][2];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  I2c.begin(); // Start I2C Master

  I2c.write(0x68, 0x00, 0x00);  // Normal mode
//  I2c.write(0x68, 0x00, 0x10);  // Sleep mode
//  I2c.write(0x68, 0x00, 0x20);  // Standby mode (60sec intermittence)
//  I2c.write(0x68, 0x00, 0x21);  // Standby mode (10sec intermittence)
  


}
void loop() {
  //put your main code here, to run repeatedly:

  for (int i = 0; i < 128; i += 2) {
    I2c.read(0x68 , 0x80 + i, 2);
    value[i / 2][0] = I2c.receive();
    value[i / 2][1] = I2c.receive();
  }



  uint16_t v1, v2, sign, fin_value;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      v1 = value[(i * 8) + j][0];
      v2 = value[(i * 8) + j][1] & 15;
      fin_value = v1 + (v2 << 8);

      if (fin_value >= 2048) {
        fin_value = -((~fin_value) + 1);
      }

      Serial.print(fin_value * 0.25);
      Serial.print(" ");

    }
    Serial.print("\n");
  }
  Serial.print("\n");
  delay(1000);
}