// Panduan mengukur titik kalibrasi sendiri lewat Serial Monitor (115200,
// akhiran baris "Newline"). Cocok untuk sensor apa saja di A0.
//
// Langkah:
// 1. Buat keadaan yang nilai nyatanya kamu tahu (mis. benda tepat 20 cm dari
//    sensor, diukur dengan penggaris; atau beban 1 kg di timbangan).
// 2. Ketik nilai nyata itu (mis. 20) lalu Enter. Bacaan A0 saat itu dicatat.
// 3. Ulangi untuk keadaan lain, berurutan dari kecil ke besar (atau sebaliknya).
//    Makin banyak titik di bagian yang melengkung, makin teliti hasilnya.
// 4. Ketik s lalu Enter. Tabel dicetak siap disalin ke sketch kamu, dan
//    setelah itu bacaan A0 langsung diubah memakai tabel tersebut.
#include <KalibrasiSensor.h>

const uint8_t MAKS_TITIK = 12;
float mentah[MAKS_TITIK];
float nyata[MAKS_TITIK];
uint8_t jumlah = 0;
bool selesai = false;
KalibrasiSensor kal(mentah, nyata, 0); // jumlah titik baru diketahui nanti

// Rata-rata 32 bacaan agar titik yang dicatat tidak ikut noise.
float bacaMentah() {
  float total = 0;
  for (uint8_t i = 0; i < 32; i++) total += analogRead(A0);
  return total / 32;
}

void cetakTabel(const char *nama, const float *isi) {
  Serial.print("const float ");
  Serial.print(nama);
  Serial.print("[] = {");
  for (uint8_t i = 0; i < jumlah; i++) {
    if (i) Serial.print(", ");
    Serial.print(isi[i], 2);
  }
  Serial.println("};");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Ketik nilai nyata lalu Enter untuk mencatat titik. Ketik s untuk selesai.");
}

void loop() {
  if (Serial.available()) {
    if (Serial.peek() == 's') {
      kal = KalibrasiSensor(mentah, nyata, jumlah);
      if (kal.valid()) {
        selesai = true;
        Serial.println("\nSalin ke sketch kamu:");
        cetakTabel("MENTAH", mentah);
        cetakTabel("NYATA", nyata);
        Serial.print("KalibrasiSensor kal(MENTAH, NYATA, ");
        Serial.print(jumlah);
        Serial.println(");\n");
      } else {
        Serial.println("Tabel belum bisa dipakai: butuh minimal 2 titik dan bacaan mentah harus");
        Serial.println("urut naik atau turun tanpa nilai kembar. Ketik r untuk mulai ulang.");
      }
    } else if (Serial.peek() == 'r') {
      jumlah = 0;
      selesai = false;
      Serial.println("Mulai ulang.");
    } else if (!selesai && jumlah < MAKS_TITIK) {
      nyata[jumlah] = Serial.parseFloat();
      mentah[jumlah] = bacaMentah();
      Serial.print("Titik ");
      Serial.print(jumlah + 1);
      Serial.print(": mentah ");
      Serial.print(mentah[jumlah], 2);
      Serial.print(" = nyata ");
      Serial.println(nyata[jumlah], 2);
      jumlah++;
    }
    while (Serial.available()) Serial.read(); // buang sisa baris
  }

  static uint32_t terakhir = 0;
  if (selesai && millis() - terakhir >= 1000) {
    terakhir = millis();
    float m = bacaMentah();
    Serial.print("mentah ");
    Serial.print(m, 1);
    Serial.print(" = ");
    Serial.println(kal.ubah(m), 2);
  }
}
