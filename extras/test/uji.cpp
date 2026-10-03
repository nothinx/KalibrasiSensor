// Uji logika KalibrasiSensor di PC:
//   g++ -std=c++11 -Wall -Wextra -I. -I../../src uji.cpp ../../src/KalibrasiSensor.cpp -o uji && ./uji
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "KalibrasiSensor.h"

static bool dekat(float a, float b) { return fabsf(a - b) < 1e-4f; }

int main() {
  int kasus = 0;
  const float mentah[] = {0, 10, 20, 40};
  const float nyata[] = {0, 100, 150, 350};
  { // tabel naik: titik persis, di antara titik, dan di luar rentang (dibatasi)
    KalibrasiSensor k(mentah, nyata, 4);
    assert(k.valid() && k.mulai());
    for (int i = 0; i < 4; i++) assert(k.ubah(mentah[i]) == nyata[i]);
    assert(dekat(k.ubah(5), 50));
    assert(dekat(k.ubah(15), 125));
    assert(dekat(k.ubah(30), 250));
    assert(k.ubah(-100) == 0);
    assert(k.ubah(1000) == 350);
    assert(isnan(k.ubah(NAN)));
    kasus++;
  }
  { // ekstrapolasi: garis ruas pertama/terakhir diteruskan
    KalibrasiSensor k(mentah, nyata, 4);
    k.aturEkstrapolasi(true);
    assert(dekat(k.ubah(-5), -50));
    assert(dekat(k.ubah(50), 450));
    assert(dekat(k.ubah(30), 250)); // di dalam rentang tidak berubah
    assert(k.ubah(40) == 350);
    kasus++;
  }
  { // tabel turun (mis. Sharp IR: tegangan makin kecil saat jarak makin jauh)
    const float volt[] = {2.75f, 1.65f, 0.95f, 0.4f};
    const float cm[] = {10, 20, 40, 80};
    KalibrasiSensor k(volt, cm, 4);
    assert(k.valid());
    for (int i = 0; i < 4; i++) assert(k.ubah(volt[i]) == cm[i]);
    assert(dekat(k.ubah(2.2f), 15));
    assert(dekat(k.ubah(0.675f), 60));
    assert(k.ubah(3.3f) == 10);
    assert(k.ubah(0.1f) == 80);
    k.aturEkstrapolasi(true);
    assert(dekat(k.ubah(0.125f), 100));
    assert(dekat(k.ubah(3.85f), 0));
    kasus++;
  }
  { // nilai nyata boleh turun walau mentah naik (mis. termistor NTC)
    const float adc[] = {200, 500, 800};
    const float suhu[] = {80, 40, 10};
    KalibrasiSensor k(adc, suhu, 3);
    assert(dekat(k.ubah(350), 60) && dekat(k.ubah(650), 25));
    kasus++;
  }
  { // tabel salah: tidak monoton, nilai kembar, NAN, titik kurang dari 2
    const float acak[] = {0, 10, 5, 20};
    const float kembar[] = {0, 10, 10, 20};
    const float adaNan[] = {0, NAN, 20, 30};
    KalibrasiSensor a(acak, nyata, 4), b(kembar, nyata, 4), c(adaNan, nyata, 4), d(mentah, nyata, 1), e(mentah, nyata, 0),
        f(nullptr, nyata, 4);
    assert(!a.valid() && !b.valid() && !c.valid() && !d.valid() && !e.valid() && !f.valid());
    assert(!a.mulai() && isnan(a.ubah(5)) && isnan(d.ubah(0)));
    kasus++;
  }
  { // tabel diubah saat berjalan (hasil kalibrasi pengguna), lalu mulai() lagi
    float m[] = {0, 0};
    const float n[] = {0, 100};
    KalibrasiSensor k(m, n, 2);
    assert(!k.valid());
    m[0] = 820; // kering
    m[1] = 380; // basah
    assert(k.mulai());
    assert(dekat(k.ubah(600), 50));
    assert(k.ubah(900) == 0 && k.ubah(100) == 100);
    kasus++;
  }
  { // kalibrasi dua titik untuk sensor linear: tabel 2 titik + ekstrapolasi
    const float baca[] = {102.0f, 845.0f}; // bacaan saat 0 kg dan 5 kg
    const float kg[] = {0, 5};
    KalibrasiSensor k(baca, kg, 2);
    k.aturEkstrapolasi(true);
    assert(k.ubah(102) == 0 && k.ubah(845) == 5);
    assert(dekat(k.ubah(473.5f), 2.5f));
    assert(dekat(k.ubah(1588), 10));
    kasus++;
  }
  { // tabel 255 titik: indeks uint8_t tidak meluap
    static float m[255], n[255];
    for (int i = 0; i < 255; i++) { m[i] = (float)i; n[i] = (float)(2 * i); }
    KalibrasiSensor k(m, n, 255);
    assert(k.valid());
    assert(k.ubah(254) == 508 && k.ubah(300) == 508 && dekat(k.ubah(253.5f), 507));
    k.aturEkstrapolasi(true);
    assert(dekat(k.ubah(300), 600));
    kasus++;
  }
  printf("Semua uji lolos (%d kasus, sizeof = %u byte)\n", kasus, (unsigned)sizeof(KalibrasiSensor));
  return 0;
}
