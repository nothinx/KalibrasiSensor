// Kelembapan tanah 0-100% dari sensor kapasitif (atau resistif) di A0.
// Cukup dua titik: bacaan saat sensor kering dan saat terendam air.
//
// Cara mendapatkan angkanya: jalankan sketch ini, lihat angka "mentah" di
// Serial Monitor (115200). Catat saat sensor di udara (KERING) dan saat
// dicelup air sampai batas garis (BASAH), lalu isi di bawah dan upload ulang.
//
// Sambungan: VCC ke 5V/3V3, GND ke GND, AOUT ke A0.
#include <KalibrasiSensor.h>

// Sensor kapasitif: makin basah makin KECIL bacaannya. Tabel turun pun boleh.
const float MENTAH[] = {520, 260}; // KERING, BASAH (contoh di Uno)
const float PERSEN[] = {0, 100};
KalibrasiSensor tanah(MENTAH, PERSEN, 2);

void setup() {
  Serial.begin(115200);
  if (!tanah.mulai()) Serial.println("KERING dan BASAH tidak boleh sama!");
}

void loop() {
  static uint32_t terakhir = 0;
  if (millis() - terakhir < 1000) return;
  terakhir = millis();

  int mentah = analogRead(A0);
  Serial.print("mentah: ");
  Serial.print(mentah);
  Serial.print("  kelembapan: ");
  Serial.print(tanah.ubah(mentah), 0); // lebih kering/basah dari titik ukur tetap 0/100
  Serial.println(" %");
}
