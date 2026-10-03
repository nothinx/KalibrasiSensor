# KalibrasiSensor (English)

[Bahasa Indonesia](README.md)

An Arduino library to **convert raw sensor readings into real values** with a calibration table (piecewise linear interpolation). Works for non-linear sensors (Li-ion battery percentage, Sharp IR distance, thermistors) and linear ones (two-point calibration). The API and examples are in Indonesian. This page maps every function to English.

```cpp
#include <KalibrasiSensor.h>

// Sharp GP2Y0A21: voltage (V) -> distance (cm). A descending table is fine.
const float VOLT[] = {2.30, 1.65, 1.30, 0.92, 0.75, 0.60, 0.50, 0.45, 0.40};
const float CM[]   = {10,   15,   20,   30,   40,   50,   60,   70,   80};
KalibrasiSensor sharp(VOLT, CM, 9);

void setup() {
  Serial.begin(115200);
  if (!sharp.mulai()) Serial.println("Bad calibration table!");   // mulai() = begin()
}

void loop() {
  float volt = analogRead(A0) * 5.0 / 1023;
  Serial.println(sharp.ubah(volt));   // ubah() = convert()
  delay(100);
}
```

## Why

- The raw column may be ascending or descending. With a descending table, MultiMap returned 10 cm for 2.0 V, 1.0 V, and 0.5 V in our PC test.
- The table is validated: `mulai()` / `valid()` return `false` for fewer than 2 points or a non-monotonic raw column, and `ubah()` then returns `NAN` instead of a plausible-looking wrong number.
- Out-of-range input is clamped (default) or extrapolated.
- Two-point calibration is just a 2-point table plus `aturEkstrapolasi(true)`.
- Tables can live in flash on AVR (`PROGMEM` + `DI_FLASH`): a 10-point table saves 80 bytes of Uno RAM.
- Accepts `const` tables. The table is not copied, so the object is 8 bytes on Uno.

## Simulation results

PC simulation calling `ubah()` with the tables from the `PersenBaterai` and `SharpIRJarak` examples (not a hardware measurement; the battery curve is an approximate community table).

![Li-ion percentage from the calibration table versus a linear 3.0 to 4.2 volt mapping](extras/gambar/baterai.svg)

A linear `map()` reports 58% at 3.70 V; the table curve says 12.5%.

![Sharp IR distance from a descending table, clamped and extrapolated outside the table](extras/gambar/sharp-ir.svg)

The descending table is used as is. Outside the table (grey) the result is clamped by default, or extrapolated with `aturEkstrapolasi(true)`.

![multiMap with a descending table returns 10 cm for almost every voltage](extras/gambar/multimap-tabel-turun.svg)

For comparison, `multiMap()` from MultiMap 0.4.0 (copied from its source into the simulation) with the same table: 10 cm for every voltage up to 2.30 V, 80 cm above. Regenerate with `cd extras/simulasi && python gambar.py` (needs g++ and matplotlib).

## Function reference

| Indonesian | English | Notes |
|---|---|---|
| `KalibrasiSensor(mentah, nyata, jumlah, lokasi)` | constructor (raw, real, count, location) | 2–255 points; `DI_RAM` (default) or `DI_FLASH` (PROGMEM) |
| `mulai()` | begin / validate | `false` if fewer than 2 points or raw values not strictly monotonic; call again after changing the table |
| `valid()` | is valid | |
| `ubah(bacaan)` | convert(reading) | `NAN` if the table is invalid |
| `aturEkstrapolasi(aktif)` | set extrapolation | `false` (default) clamps to the end points |

## Examples

`PersenBaterai` (Li-ion percentage, approximate community curve, table in flash), `KelembapanTanahPersen` (soil moisture dry/wet → %), `SharpIRJarak` (Sharp IR distance), `KalibrasiDuaTitik` (two-point linear calibration), `MencatatTitikKalibrasi` (record your own calibration points from the Serial Monitor).

## Status

Version 1.0.0 passes automated logic tests on PC and compiles without warnings on Uno, ESP32, and STM32 Bluepill (CI covers 7 boards). The AVR `DI_FLASH` path has only been compile-tested, **not run on real hardware**.

## License

MIT © 2026 Amadeo Wisesa.
