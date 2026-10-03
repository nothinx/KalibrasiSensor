// Jarak dari sensor IR Sharp GP2Y0A21YK0F (10-80 cm). Tegangan keluarannya
// TURUN saat jarak makin jauh dan tidak linear.
//
// Titik tabel dibaca kira-kira dari grafik datasheet ("Analog output voltage
// vs distance to reflective object"). Tiap sensor sedikit berbeda, jadi untuk
// hasil teliti ukur sendiri dengan penggaris (lihat MencatatTitikKalibrasi).
// Di bawah 10 cm tegangannya turun lagi, sehingga benda terlalu dekat terbaca jauh.
//
// Sambungan: merah ke 5V, hitam ke GND, kuning ke A0. Pasang kapasitor
// 10-100 uF di dekat sensor antara 5V dan GND untuk meredam lonjakan arus.
#include <KalibrasiSensor.h>

const float VOLT[] = {2.30, 1.65, 1.30, 0.92, 0.75, 0.60, 0.50, 0.45, 0.40};
const float CM[] = {10, 15, 20, 30, 40, 50, 60, 70, 80};
KalibrasiSensor sharp(VOLT, CM, 9);

void setup() {
  Serial.begin(115200);
  if (!sharp.mulai()) Serial.println("Tabel kalibrasi salah!");
}

void loop() {
  static uint32_t terakhir = 0;
  if (millis() - terakhir < 100) return;
  terakhir = millis();

#if defined(ESP32)
  float volt = analogReadMilliVolts(A0) / 1000.0; // keluaran sensor maks 3,1 V, aman untuk ESP32
#elif defined(__AVR__)
  float volt = analogRead(A0) * 5.0 / 1023;
#else
  float volt = analogRead(A0) * 3.3 / 1023;
#endif
  Serial.print(volt, 2);
  Serial.print(" V = ");
  Serial.print(sharp.ubah(volt), 1);
  Serial.println(" cm");
}
