#!/usr/bin/env python3
"""
sweep_offset.py  –  batch-driver for muAlphaSim

  • generates geometry.json with a variable X-offset for *panel 0*
  • runs the executable, passing the json path via --cfg argument
  • collects every result_*/run_*.json into one CSV for quick plotting

USAGE
-----
  ./sweep_offset.py -n 1e5  --offsets -300 -150 0 150 300

Dependencies: Python 3.8+, pandas
"""

import argparse
import json
import os
import shutil
import subprocess
import time
from pathlib import Path

import pandas as pd


# ────────────────────────────────────────────────────────────────────────────
#  Helpers
# ────────────────────────────────────────────────────────────────────────────
def make_geometry(offset_nm: float) -> dict:
    """Return a GeometryConfig dict with panel[0].x0_nm = offset_nm."""
    return {
        "cone": {"r_tip_nm": 0.5, "r_base_nm": 10.0, "h_cone_nm": 1000.0},
        "gap_nm": 50.0,
        "r_middle_nm": 60.0,
        "r_outer_nm": 75.0,
        "panels": [
            {
                "nx": 5,
                "ny": 5,
                "pitch_nm": 150.0,
                "x0_nm": offset_nm,
                "offset_nm": [0.0, 0.0, 0.0],
            },
            {
                "nx": 5,
                "ny": 5,
                "pitch_nm": 225.0,
                "x0_nm": offset_nm + 1000.0,
                "offset_nm": [0.0, 0.0, 0.0],
            },
        ],
    }


def run_one(offset: float, n_events: int, exe: Path, out_root: Path) -> Path:
    """Write geometry, run simulation, return path to run summary json."""
    workdir = out_root / f"offset_{offset:+.0f}nm"
    workdir.mkdir(parents=True, exist_ok=True)

    geom_path = workdir / "geometry.json"
    with geom_path.open("w") as fp:
        json.dump(make_geometry(offset), fp, indent=2)

    cmd = [
        str(exe),
        f"--cfg={geom_path}",      # option you add in main() to load JSON
        f"--nevents={n_events}",   # likewise parsed in main()
    ]
    print(" ".join(cmd))
    subprocess.run(cmd, cwd=workdir, check=True)

    # DataLogger writes results/run_*.json inside workdir
    res_dir = workdir / "results"
    latest = max(res_dir.glob("run_*.json"), key=os.path.getmtime)
    return latest


# ────────────────────────────────────────────────────────────────────────────
#  CLI & main loop
# ────────────────────────────────────────────────────────────────────────────
def parse_cli():
    ap = argparse.ArgumentParser()
    ap.add_argument("-n", "--nevents", type=int, default=100000,
                    help="events per run (default: 1e5)")
    ap.add_argument("--exe", default="./build/main",
                    help="path to muAlphaSim executable")
    ap.add_argument("--offsets", nargs="+", type=float, required=True,
                    help="list of x-offsets (nm) to sweep")
    ap.add_argument("--out", default="sweep_results",
                    help="root directory for results")
    return ap.parse_args()


def main():
	args = parse_cli()
	exe = Path(args.exe).resolve()
	root = Path(args.out).resolve()
	shutil.rmtree(root, ignore_errors=True)
	root.mkdir()

	json_paths = []
	for off in args.offsets:
		json_path = run_one(off, args.nevents, exe, root)
		json_paths.append(json_path)

    # aggregate CSV
    # rows = [json.load(open(p)) | {"offset_nm": float(p.parent.name.split("_")[1][:-2])}
    #         for p in json_paths]
    # df = pd.DataFrame(rows)
    # df.to_csv(root / "summary.csv", index=False)
    
	rows = []
	for off, p in zip(args.offsets, json_paths):
		with open(p) as fp:
			d = json.load(fp)
		d["offset_nm"] = off
		rows.append(d)

	df = pd.DataFrame(rows)
	df.to_csv(root / "summary.csv", index=False)

	print("Wrote", root / "summary.csv")


if __name__ == "__main__":
    main()