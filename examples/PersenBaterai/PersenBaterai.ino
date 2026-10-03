// Persen baterai Li-ion 1 sel (3,7 V) dari tegangannya. Hubungan tegangan dan
// sisa muatan tidak linear: 3,7-3,9 V mencakup hampir separuh kapasitas, jadi
// map() biasa menghasilkan persen yang menyesatkan.
//
// Kurva di bawah adalah PERKIRAAN dari tabel yang umum dipakai komunitas untuk
// sel Li-ion/LiPo 3,7 V saat istirahat (tanpa beban, suhu ruang), bukan dari
// datasheet sel tertentu. Saat motor menyala tegangan turun, sehingga persen
// ikut turun sementara. Untuk hasil terbaik, ukur kurva sel kamu sendiri
// (lihat contoh MencatatTitikKalibrasi).
//
// Sambungan: pembagi tegangan 2 resistor sama besar (mis. 100k + 100k):
// (+) baterai - 100k - A0 - 100k - GND. Tegangan di A0 = setengah tegangan baterai.
#include <KalibrasiSensor.h>

// 21 titik x 2 x 4 byte = 168 byte. Disimpan di flash agar RAM Uno tetap lega.
const float TEGANGAN[] PROGMEM = {3.27, 3.61, 3.69, 3.71, 3.73, 3.75, 3.77, 3.79, 3.80, 3.82, 3.84,
                                  3.85, 3.87, 3.91, 3.95, 3.98, 4.02, 4.08, 4.11, 4.15, 4.20};
const float PERSEN[] PROGMEM = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50,
                                55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
KalibrasiSensor baterai(TEGANGAN, PERSEN, 21, KalibrasiSensor::DI_FLASH);

void setup() {
  Serial.begin(115200);
  if (!baterai.mulai()) Serial.println("Tabel kalibrasi salah!");
}

float bacaTegangan() {
  float total = 0;
  for (uint8_t i = 0; i < 16; i++) { // rata-rata 16 bacaan untuk meredam noise ADC
#if defined(ESP32)
    total += analogReadMilliVolts(A0) / 1000.0;
#elif defined(__AVR__)
    total += analogRead(A0) * 5.0 / 1023;
#else
    total += analogRead(A0) * 3.3 / 1023;
#endif
  }
  return total / 16 * 2; // x2 karena pembagi tegangan
}

void loop() {
  static uint32_t terakhir = 0;
  if (millis() - terakhir < 1000) return;
  terakhir = millis();

  float volt = bacaTegangan();
  Serial.print(volt, 2);
  Serial.print(" V = ");
  Serial.print(baterai.ubah(volt), 0); // di luar 3,27-4,20 V dibatasi ke 0 atau 100
  Serial.println(" %");
}
