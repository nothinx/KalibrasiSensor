"""Compile + jalankan simulasi.cpp (kode library asli), lalu render grafik ke ../gambar/.

Jalankan dari folder ini:  python gambar.py   (butuh g++ dan matplotlib)
"""
import glob
import os
import subprocess
import sys
import tempfile

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

plt.rcParams.update({
    "figure.figsize": (8, 3.6), "figure.dpi": 100, "savefig.bbox": "tight", "savefig.pad_inches": 0.15,
    "figure.facecolor": "white", "axes.facecolor": "white", "savefig.facecolor": "white",
    "font.size": 10, "axes.titlesize": 11, "axes.titleweight": "bold", "axes.titlelocation": "left",
    "axes.spines.top": False, "axes.spines.right": False, "axes.edgecolor": "#9ca3af",
    "axes.grid": True, "grid.color": "#e5e7eb", "grid.linewidth": 0.8,
    "legend.frameon": False, "svg.fonttype": "path", "svg.hashsalt": "nothinx",
    "lines.linewidth": 1.8,
})
WARNA = {"utama": "#2563eb", "pembanding": "#dc2626", "ketiga": "#16a34a", "keempat": "#9333ea",
         "kelima": "#ea580c", "mentah": "#9ca3af", "target": "#111827"}

SINI = os.path.dirname(os.path.abspath(__file__))
KELUAR = os.path.join(SINI, "..", "gambar")


def koma(x, d=0):
    return f"{x:.{d}f}".replace(".", ",")


def jalankan():
    with tempfile.TemporaryDirectory() as tmp:
        exe = os.path.join(tmp, "sim")
        src = glob.glob(os.path.join(SINI, "..", "..", "src", "*.cpp"))
        hasil = subprocess.run(["g++", "-std=c++11", "-O2", "-I../test", "-I../../src", "simulasi.cpp", *src,
                                "-o", exe], cwd=SINI, capture_output=True, text=True)
        if hasil.returncode:
            sys.exit(hasil.stderr)
        teks = subprocess.run([exe], check=True, capture_output=True, text=True).stdout
    data, kolom = {}, None
    for baris in teks.splitlines():
        if baris.startswith("# "):
            nama, kolom = baris[2:], None
            data[nama] = {}
        elif kolom is None:
            kolom = baris.split(",")
            data[nama] = {k: [] for k in kolom}
        else:
            for k, v in zip(kolom, baris.split(",")):
                data[nama][k].append(float(v))
    return data


def simpan(fig, nama):
    fig.savefig(os.path.join(KELUAR, nama), format="svg", metadata={"Date": None})
    plt.close(fig)


def di(seri, x, kolom):
    """Nilai kolom pada x (grid 0,01 V)."""
    return seri[kolom][min(range(len(seri["volt"])), key=lambda i: abs(seri["volt"][i] - x))]


def baterai(d):
    b, t = d["baterai"], d["baterai_titik"]
    fig, ax = plt.subplots()
    # Pemetaan linear 3,0-4,2 V -> 0-100% yang lazim, seperti map(mV, 3000, 4200, 0, 100).
    lin = lambda v: (v - 3.0) / 1.2 * 100
    ax.plot([3.0, 4.2], [lin(3.0), lin(4.2)], color=WARNA["pembanding"], linestyle="--", linewidth=1.5)
    ax.plot(b["volt"], b["persen"], color=WARNA["utama"])
    ax.plot(t["volt"], t["persen"], "o", color=WARNA["utama"], markersize=4.5, markerfacecolor="white")
    v = 3.70
    p = di(b, v, "persen")
    ax.annotate("", (v, p), xytext=(v, lin(v)), arrowprops=dict(arrowstyle="<->", color=WARNA["target"], lw=1))
    ax.text(v - 0.02, (p + lin(v)) / 2, f"{koma(v, 2)} V:\nlinear {koma(lin(v))}%\ntabel {koma(p, 1)}%",
            ha="right", va="center", fontsize=9, color=WARNA["target"])
    ax.text(3.02, 36, "map() linear 3,0–4,2 V", color=WARNA["pembanding"], fontsize=9)
    ax.text(3.97, 52, "KalibrasiSensor.ubah()\n○ titik tabel", color=WARNA["utama"], fontsize=9)
    ax.set_xlabel("Tegangan sel (V)")
    ax.set_ylabel("Sisa muatan (%)")
    ax.set_xlim(3.0, 4.3)
    ax.set_ylim(-3, 105)
    ax.set_title(f"Di {koma(v, 2)} V, pemetaan linear bilang {koma(lin(v))}%, kurva tabel bilang {koma(p, 1)}%")
    simpan(fig, "baterai.svg")


def sharp(d):
    s, t = d["sharp"], d["sharp_titik"]
    lo, hi = min(t["volt"]), max(t["volt"])
    fig, ax = plt.subplots()
    for a, b in ((0, lo), (hi, 3.0)):
        ax.axvspan(a, b, color="#f3f4f6", linewidth=0)
    ax.plot(s["volt"], s["ekstrapolasi"], color=WARNA["kelima"], linestyle="--", linewidth=1.5)
    ax.plot(s["volt"], s["batas"], color=WARNA["utama"])
    ax.plot(t["volt"], t["cm"], "o", color=WARNA["utama"], markersize=4.5, markerfacecolor="white")
    v = 0.20
    e, b = di(s, v, "ekstrapolasi"), di(s, v, "batas")
    ax.text(v + 0.04, e, f"ekstrapolasi: {koma(e)} cm", color=WARNA["kelima"], fontsize=9, va="center")
    ax.plot([v, v], [e, b], "o", color=WARNA["target"], markersize=4)
    ax.text(0.5, 85, f"dibatasi (default): {koma(b)} cm", color=WARNA["utama"], fontsize=9, va="center")
    ax.text(1.0, 50, "○ titik tabel", color=WARNA["utama"], fontsize=9)
    ax.text(lo / 2, 3, "di luar\ntabel", ha="center", color=WARNA["mentah"], fontsize=8.5)
    ax.text((hi + 3) / 2, 25, "di luar tabel", ha="center", color=WARNA["mentah"], fontsize=8.5)
    ax.set_xlabel("Tegangan keluaran sensor (V)")
    ax.set_ylabel("Jarak (cm)")
    ax.set_xlim(0, 3.0)
    ax.set_ylim(0, 170)
    ax.set_title(f"Sharp IR (tabel turun): di {koma(v, 2)} V hasil dibatasi {koma(b)} cm "
                 f"atau diteruskan {koma(e)} cm")
    simpan(fig, "sharp-ir.svg")


def multimap(d):
    s, t = d["sharp"], d["sharp_titik"]
    fig, ax = plt.subplots()
    ax.plot(s["volt"], s["multimap"], color=WARNA["pembanding"])
    ax.plot(s["volt"], s["batas"], color=WARNA["utama"])
    ax.plot(t["volt"], t["cm"], "o", color=WARNA["utama"], markersize=4.5, markerfacecolor="white")
    for v in (0.5, 1.0, 2.0):
        ax.plot(v, di(s, v, "multimap"), "o", color=WARNA["pembanding"], markersize=5)
    ax.text(0.9, 50, "KalibrasiSensor.ubah()", color=WARNA["utama"], fontsize=9)
    ax.text(1.05, 4, "multiMap() dengan tabel yang sama", color=WARNA["pembanding"], fontsize=9)
    ax.set_xlabel("Tegangan keluaran sensor (V)")
    ax.set_ylabel("Jarak (cm)")
    ax.set_xlim(0, 3.0)
    ax.set_ylim(0, 90)
    hasil = {koma(di(s, v, "multimap")) for v in (0.5, 1.0, 2.0)}
    ax.set_title(f"MultiMap 0.4.0 dengan tabel turun: {'/'.join(sorted(hasil))} cm untuk 0,5 V, 1,0 V, dan 2,0 V")
    simpan(fig, "multimap-tabel-turun.svg")


def main():
    os.makedirs(KELUAR, exist_ok=True)
    d = jalankan()
    baterai(d)
    sharp(d)
    multimap(d)


if __name__ == "__main__":
    main()
