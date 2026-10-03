// KalibrasiSensor - ubah bacaan mentah sensor menjadi nilai nyata lewat tabel
// titik ukur (interpolasi linear antar titik). Untuk sensor tidak linear
// (persen baterai, Sharp IR, termistor) maupun linear (cukup 2 titik).
// Copyright (c) 2026 Amadeo Wisesa. Lisensi MIT.
//
// - Tabel mentah boleh naik atau turun, asal urut (monoton).
// - Di luar rentang tabel: dibatasi ke titik ujung (default) atau ekstrapolasi.
// - Tabel tidak disalin: hemat RAM, dan isinya boleh diubah saat berjalan
//   (panggil mulai() lagi setelahnya). Di AVR tabel bisa disimpan di flash (PROGMEM).
#pragma once
#include <Arduino.h>

class KalibrasiSensor {
public:
  enum Lokasi : uint8_t {
    DI_RAM,  // tabel biasa (default)
    DI_FLASH // tabel ber-PROGMEM, hemat RAM di AVR (Uno, Nano, Mega)
  };

  // mentah[] dan nyata[] berisi jumlah titik, berpasangan. Tabel tidak disalin,
  // jadi harus tetap ada (global atau static).
  KalibrasiSensor(const float *mentah, const float *nyata, uint8_t jumlah, Lokasi lokasi = DI_RAM)
      : _mentah(mentah), _nyata(nyata), _jumlah(jumlah), _flash(lokasi == DI_FLASH) { mulai(); }

  // Periksa tabel. false jika titik < 2 atau mentah[] tidak urut naik/turun
  // (ada nilai kembar). Sudah dipanggil oleh constructor; panggil lagi
  // setelah isi tabel diubah.
  bool mulai();
  bool valid() const { return _valid; }

  // Bacaan mentah -> nilai nyata. NAN jika tabel tidak valid.
  float ubah(float bacaan) const;

  // false (default): di luar rentang tabel hasil dibatasi ke nilai ujung.
  // true: garis dua titik terdekat diteruskan (cocok untuk sensor linear).
  void aturEkstrapolasi(bool aktif) { _ekstrapolasi = aktif; }

private:
  float baca(const float *tabel, uint8_t i) const;

  const float *_mentah;
  const float *_nyata;
  uint8_t _jumlah;
  bool _flash;
  bool _valid = false;
  bool _ekstrapolasi = false;
};
