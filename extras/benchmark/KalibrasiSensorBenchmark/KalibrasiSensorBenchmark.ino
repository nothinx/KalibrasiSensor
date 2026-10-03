// Benchmark KalibrasiSensor di ATmega328P 16 MHz (simavr). Cara menjalankan dan
// angka hasilnya: README bagian "Kecepatan & memori".
// Siklus.h: Timer1 tanpa prescaler, UKUR(nama, ulang, kode) mencetak
// "BENCH nama siklus_per_panggilan".
#include <KalibrasiSensor.h>
#include "Siklus.h"

float m2[2] = {0, 1023}, n2[2] = {0, 100};
float m8[8] = {3.0, 3.3, 3.5, 3.6, 3.7, 3.8, 4.0, 4.2}, n8[8] = {0, 5, 10, 20, 40, 60, 85, 100};
const float f8[8] PROGMEM = {3.0, 3.3, 3.5, 3.6, 3.7, 3.8, 4.0, 4.2}, g8[8] PROGMEM = {0, 5, 10, 20, 40, 60, 85, 100};
float m32[32], n32[32];
KalibrasiSensor k2(m2, n2, 2), k8(m8, n8, 8), kFlash8(f8, g8, 8, KalibrasiSensor::DI_FLASH), k32(m32, n32, 32);
// Masukan bergilir di seluruh rentang agar semua ruas terkena.
float x2[8] = {10, 150, 300, 450, 600, 750, 900, 1000}, x8[8] = {3.05, 3.4, 3.55, 3.65, 3.75, 3.9, 4.1, 4.15}, x32[8];
volatile float hasil;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 32; i++) { m32[i] = i * 32; n32[i] = sqrt(i * 32.0); }
  for (int i = 0; i < 8; i++) x32[i] = 16 + i * 128;
  k32.mulai();
  Serial.print(F("BENCH sizeof "));
  Serial.println(sizeof(KalibrasiSensor));
  UKUR("kosong", 1000, hasil = x8[_i & 7]);
  UKUR("ubah_2_titik", 1000, hasil = k2.ubah(x2[_i & 7]));
  UKUR("ubah_8_titik", 1000, hasil = k8.ubah(x8[_i & 7]));
  UKUR("ubah_8_titik_PROGMEM", 1000, hasil = kFlash8.ubah(x8[_i & 7]));
  UKUR("ubah_32_titik", 1000, hasil = k32.ubah(x32[_i & 7]));
  selesai();
}

void loop() {}
