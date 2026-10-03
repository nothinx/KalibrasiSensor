#include "KalibrasiSensor.h"

float KalibrasiSensor::baca(const float *tabel, uint8_t i) const {
#ifdef __AVR__
  if (_flash) return pgm_read_float(tabel + i);
#endif
  return tabel[i]; // selain AVR, flash bisa dibaca langsung
}

bool KalibrasiSensor::mulai() {
  _valid = false;
  if (_jumlah < 2 || !_mentah || !_nyata) return false;
  bool naik = _naik = baca(_mentah, 1) > baca(_mentah, 0);
  for (uint8_t i = 1; i < _jumlah; i++) {
    float a = baca(_mentah, i - 1), b = baca(_mentah, i);
    if (naik ? !(b > a) : !(b < a)) return false; // juga menolak NAN
  }
  _valid = true;
  return true;
}

float KalibrasiSensor::ubah(float x) const {
  if (!_valid) return NAN;
  const uint8_t akhir = _jumlah - 1;
  const bool naik = _naik;
  // x sudah melewati titik ke-i (searah tabel). Tanpa perkalian dengan arah,
  // karena perkalian float di AVR jauh lebih mahal daripada pembanding.
  auto lewat = [&](uint8_t i) { float m = baca(_mentah, i); return naik ? x > m : x < m; };
  if (!_ekstrapolasi) {
    // Ditulis sebagai pembanding langsung agar NAN lolos ke bawah dan menghasilkan NAN.
    float m = baca(_mentah, 0);
    if (naik ? x <= m : x >= m) return baca(_nyata, 0);
    m = baca(_mentah, akhir);
    if (naik ? x >= m : x <= m) return baca(_nyata, akhir);
  }
  // Binary search ruas: i terbesar di 0..akhir-1 dengan x melewati titik i.
  // Di luar rentang memakai ruas pertama/terakhir (ekstrapolasi). NAN: ruas 0.
  uint8_t i = 0, j = akhir - 1;
  while (i < j) {
    uint8_t t = (i + j + 1) / 2;
    if (lewat(t)) i = t;
    else j = t - 1;
  }
  float m0 = baca(_mentah, i), m1 = baca(_mentah, i + 1);
  float t = (x - m0) / (m1 - m0);
  return baca(_nyata, i) * (1 - t) + baca(_nyata, i + 1) * t; // tepat di titik ukur saat t = 0 atau 1
}
