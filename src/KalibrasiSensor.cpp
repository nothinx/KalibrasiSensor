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
  bool naik = baca(_mentah, 1) > baca(_mentah, 0);
  for (uint8_t i = 1; i < _jumlah; i++) {
    float a = baca(_mentah, i - 1), b = baca(_mentah, i);
    if (naik ? !(b > a) : !(b < a)) return false; // juga menolak NAN
  }
  _valid = true;
  return true;
}

float KalibrasiSensor::ubah(float x) const {
  if (!_valid) return NAN;
  uint8_t akhir = _jumlah - 1;
  // Ubah tabel turun menjadi naik dengan membalik tanda, supaya satu jalur saja.
  float arah = baca(_mentah, 1) > baca(_mentah, 0) ? 1 : -1;
  float xs = x * arah;
  if (!_ekstrapolasi) {
    if (xs <= baca(_mentah, 0) * arah) return baca(_nyata, 0);
    if (xs >= baca(_mentah, akhir) * arah) return baca(_nyata, akhir);
  }
  // Cari ruas: di luar rentang memakai ruas pertama/terakhir (ekstrapolasi).
  uint8_t i = 0;
  while (i + 1 < akhir && xs > baca(_mentah, i + 1) * arah) i++;
  float m0 = baca(_mentah, i), m1 = baca(_mentah, i + 1);
  float t = (x - m0) / (m1 - m0);
  return baca(_nyata, i) * (1 - t) + baca(_nyata, i + 1) * t; // tepat di titik ukur saat t = 0 atau 1
}
