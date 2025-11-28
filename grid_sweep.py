#!/usr/bin/env python3
"""
grid_sweep.py – brute-force every 1 ≤ nx,ny ≤ 5 (25 jobs total).

  python grid_sweep.py -n 25000 \
      --exe ./build/muAlphaSim \
      --pitch 150          # nm
"""

import argparse, json, subprocess, shutil, os, time
from pathlib import Path
import pandas as pd


# ───────────────────────────────── helpers ──────────────────────────────────
def geom_cfg(nx:int, ny:int, pitch_nm:float) -> dict:
    """Return a single-panel GeometryConfig dict."""
    return {
        "cone":   {"r_tip_nm":0.5, "r_base_nm":10, "h_cone_nm":1000},
        "gap_nm": 50,
        "r_middle_nm": 60,
        "r_outer_nm": 75,
        "panels": [{
            "nx": nx, "ny": ny,
            "pitch_nm": pitch_nm,
            "x0_nm": 0.0,
            "offset_nm": [0,0,0]
        }]
    }


def run_one(nx:int, ny:int, nev:int, exe:Path, out_root:Path) -> Path:
    wdir = out_root / f"nx{nx}_ny{ny}"
    wdir.mkdir(parents=True, exist_ok=True)

    cfg_path = wdir / "geometry.json"
    cfg = geom_cfg(nx, ny, args.pitch)
    cfg_path.write_text(json.dumps(cfg, indent=2))

    cmd = [str(exe), f"--cfg={cfg_path}", f"--nevents={nev}"]
    print(" ".join(cmd))
    subprocess.run(cmd, cwd=wdir, check=True)

    res_dir = wdir / "results"
    return max(res_dir.glob("run_*.json"), key=os.path.getmtime)


# ───────────────────────────────── CLI ───────────────────────────────────────
ap = argparse.ArgumentParser()
ap.add_argument("-n", "--nevents", type=int, default=25000)
ap.add_argument("--exe", default="./build/main")
ap.add_argument("--out", default="grid_sweep")
ap.add_argument("--pitch", type=float, default=150.0,
                help="lattice pitch [nm]")
args = ap.parse_args()

exe  = Path(args.exe).resolve()
root = Path(args.out).resolve()
shutil.rmtree(root, ignore_errors=True); root.mkdir()

json_runs = []
for nx in range(1,6):
    for ny in range(1,6):
        json_runs.append(run_one(nx, ny, args.nevents, exe, root))

rows = []
for p in json_runs:
	with p.open() as fp: rec = json.load(fp)
    # tokens   = p.parents[0].name.split("_")  # nx{nx}_ny{ny}
    # rec["nx"] = int(tokens[0][2:]); rec["ny"] = int(tokens[1][2:])
	folder = p.parent.parent               # …/nx<nx>_ny<ny>
	nx_str, ny_str = folder.name.split("_")  # "nx2", "ny5"
	rec["nx"] = int(nx_str[2:])
	rec["ny"] = int(ny_str[2:])
	rows.append(rec)

pd.DataFrame(rows).to_csv(root / "summary.csv", index=False)
print("★ Wrote", root / "summary.csv")