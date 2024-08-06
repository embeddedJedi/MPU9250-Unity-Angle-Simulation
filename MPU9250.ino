#include <Wire.h>

const int MPU_addr=0x68;  // MPU9250 I2C adresi

int16_t accelerometer_x, accelerometer_y, accelerometer_z;  // ivme değerleri
int16_t gyro_x, gyro_y, gyro_z;  // açısal hız değerleri
float Acc_x, Acc_y, Acc_z;
float Gyro_x, Gyro_y, Gyro_z;
float x_angle, y_angle, z_angle;
float pool[4][3] = {{.0, .0, .0}, {.0, .0, .0}, {.0, .0, .0}, {.0, .0, .0}};

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 kaydı
  Wire.write(0);     // uyandır ma işlemi
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // ilk accelerometer kaydı adresi
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);  // 14 adet okuma yap

  // okunan değerlerin ayıklanması
  accelerometer_x = Wire.read()<<8|Wire.read();
  accelerometer_y = Wire.read()<<8|Wire.read();
  accelerometer_z = Wire.read()<<8|Wire.read();
  Wire.read();
  gyro_x = Wire.read()<<8|Wire.read();
  gyro_y = Wire.read()<<8|Wire.read();
  gyro_z = Wire.read()<<8|Wire.read();

  // ivme ölçer verilerinin işlenmesi
  Acc_x = accelerometer_x/16384.0;
  Acc_y = accelerometer_y/16384.0;
  Acc_z = accelerometer_z/16384.0;

  // açısal hız verilerinin işlenmesi
  Gyro_x = gyro_x/131.0;
  Gyro_y = gyro_y/131.0;
  Gyro_z = gyro_z/131.0;

  // açısal verilerin hesaplanması
  x_angle = atan2(Acc_y, sqrt(Acc_x*Acc_x + Acc_z*Acc_z)) * 180.0 / PI;
  y_angle = atan2(-Acc_x, sqrt(Acc_y*Acc_y + Acc_z*Acc_z)) * 180.0 / PI;
  z_angle = Gyro_z * 0.001 * 180.0 / PI;

  float pool2[4][3] = {{pool[1][0], pool[1][1], pool[1][2]}, {pool[2][0], pool[2][1], pool[2][2]}, {pool[3][0], pool[3][1], pool[3][2]}, {x_angle, y_angle, z_angle}};

 for (int i = 0; i < 4; i++) {
  for (int j = 0; j < 3; j++) {
    pool[i][j] = pool2[i][j];
  }
}
  x_angle = (pool[0][0] + pool[1][0] + pool[2][0] + pool[3][0]) / 4.0;

  y_angle = (pool[0][1] + pool[1][1] + pool[2][1] + pool[3][1]) / 4.0;
  z_angle = (pool[0][2] + pool[1][2] + pool[2][2] + pool[3][2]) / 4.0;

  // okunan verilerin seri bağlantı ile gösterilmesi
  Serial.print(x_angle);
  Serial.print(",");
  Serial.print(y_angle);
  Serial.print(",");
  Serial.println(z_angle);

  delay(40);

}
