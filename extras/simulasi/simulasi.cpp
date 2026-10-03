// Simulasi KalibrasiSensor di PC memakai kode library asli (../../src). Dipanggil oleh gambar.py.
//   g++ -std=c++11 -O2 -I../test -I../../src simulasi.cpp ../../src/*.cpp -o sim && ./sim
// Keluaran: beberapa bagian CSV, tiap bagian diawali "# nama" lalu baris nama kolom.
#include <stdio.h>
#include "KalibrasiSensor.h"

// multiMap() disalin apa adanya dari library MultiMap 0.4.0 oleh Rob Tillaart (MIT),
// https://github.com/RobTillaart/MultiMap/blob/master/MultiMap.h. Hanya untuk pembanding.
template<typename T>
T multiMap(T value, T* _in, T* _out, uint16_t size)
{
  //  output is constrained to out array
  if (value <= _in[0]) return _out[0];
  if (value >= _in[size-1]) return _out[size-1];

  //  search right interval
  uint16_t pos = 1;  // _in[0] already tested
  while(value > _in[pos]) pos++;

  //  this will handle all exact "points" in the _in array
  if (value == _in[pos]) return _out[pos];

  //  interpolate in the right segment for the rest
  //  use a modified formula for decreasing output array segment
  //  to prevent unsigned "underflow/overflow" (issue #15)
  if (_out[pos] >= _out[pos-1])
  {
    return _out[pos-1] + (value - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]);
  }
  else
  {
    return _out[pos-1] - (value - _in[pos-1]) * (_out[pos-1] - _out[pos]) / (_in[pos] - _in[pos-1]);
  }
}

// Tabel sama dengan examples/PersenBaterai (perkiraan kurva komunitas, bukan datasheet).
const float TEGANGAN[] = {3.27, 3.61, 3.69, 3.71, 3.73, 3.75, 3.77, 3.79, 3.80, 3.82, 3.84,
                          3.85, 3.87, 3.91, 3.95, 3.98, 4.02, 4.08, 4.11, 4.15, 4.20};
const float PERSEN[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50,
                        55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
// Tabel sama dengan examples/SharpIRJarak (tegangan turun saat jarak bertambah).
float VOLT[] = {2.30, 1.65, 1.30, 0.92, 0.75, 0.60, 0.50, 0.45, 0.40};
float CM[] = {10, 15, 20, 30, 40, 50, 60, 70, 80};

int main() {
  KalibrasiSensor baterai(TEGANGAN, PERSEN, 21);
  printf("# baterai_titik\nvolt,persen\n");
  for (int i = 0; i < 21; i++) printf("%.2f,%g\n", TEGANGAN[i], PERSEN[i]);
  printf("# baterai\nvolt,persen\n");
  for (int i = 0; i <= 140; i++) {
    float v = 3.0f + i * 0.01f; // 3,00 - 4,40 V
    printf("%.2f,%.4f\n", v, baterai.ubah(v));
  }

  KalibrasiSensor batas(VOLT, CM, 9), ekstra(VOLT, CM, 9);
  ekstra.aturEkstrapolasi(true);
  printf("# sharp_titik\nvolt,cm\n");
  for (int i = 0; i < 9; i++) printf("%.2f,%g\n", VOLT[i], CM[i]);
  printf("# sharp\nvolt,batas,ekstrapolasi,multimap\n");
  for (int i = 0; i <= 300; i++) {
    float v = i * 0.01f; // 0,00 - 3,00 V
    printf("%.2f,%.4f,%.4f,%.4f\n", v, batas.ubah(v), ekstra.ubah(v), multiMap<float>(v, VOLT, CM, 9));
  }
  return 0;
}
