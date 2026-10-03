// Kalibrasi dua titik untuk sensor linear: sensor tekanan, sensor arus,
// pembagi tegangan, sensor jarak ultrasonik analog, dan sejenisnya.
// Ukur bacaan mentah pada dua keadaan yang nilai nyatanya diketahui, lalu
// semua bacaan lain dihitung dari garis lurus yang melewati kedua titik.
//
// Contoh di bawah: sensor tekanan 0,5-4,5 V untuk 0-1,2 MPa. Bacaan ADC
// diukur sendiri saat tekanan 0 (selang terbuka) dan saat manometer acuan
// menunjukkan 0,8 MPa.
//
// Sambungan: merah ke 5V, hitam ke GND, kuning (sinyal) ke A0.
#include <KalibrasiSensor.h>

const float BACAAN[] = {103, 785}; // bacaan analogRead() di kedua titik
const float MPA[] = {0.0, 0.8};    // nilai nyata di kedua titik
KalibrasiSensor tekanan(BACAAN, MPA, 2);

void setup() {
  Serial.begin(115200);
  // Sensor linear: teruskan garisnya di luar kedua titik, jangan dibatasi.
  tekanan.aturEkstrapolasi(true);
  if (!tekanan.mulai()) Serial.println("Kedua bacaan ADC tidak boleh sama!");
}

void loop() {
  static uint32_t terakhir = 0;
  if (millis() - terakhir < 500) return;
  terakhir = millis();

  int adc = analogRead(A0);
  Serial.print("ADC: ");
  Serial.print(adc);
  Serial.print("  tekanan: ");
  Serial.print(tekanan.ubah(adc), 3);
  Serial.println(" MPa");
}
