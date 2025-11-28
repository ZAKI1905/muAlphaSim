#!/usr/bin/env python3
"""
ionization_histograms.py

Read `ionization_events.tsv` and generate three figures:
  1. Histogram of rho_local[m]
  2. Histogram of z_local[m]
  3. Histogram of cone_id  (ionization count per cone)

Usage
-----
python ionization_histograms.py [-f FILE] [-n NBINS] [-o OUTDIR]

  -f / --file   : path to the TSV file                     (default: ionization_events.tsv)
  -n / --nbins  : number of bins for rho and z histograms  (default: 50)
  -o / --outdir : optional directory to save PNGs instead of popping up            “plots”
"""
import argparse
from pathlib import Path

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

import re

def parse_cone_centers(path: Path) -> dict[int, tuple[float, float]]:
    """
    Extract cone_id → (x, y) pairs from the big comment header.
    """
    centres: dict[int, tuple[float, float]] = {}
    rx = re.compile(
        r"#\s*cone(\d+)\s+center\s*=\s*\(([-\d.eE]+)\s*m,\s*([-\d.eE]+)\s*m\)",
        re.IGNORECASE,
    )

    with path.open() as fh:
        for line in fh:
            m = rx.search(line)           # ← search, not match
            if m:
                cid, x, y = int(m[1]), float(m[2]), float(m[3])
                centres[cid] = (x, y)
            # no `break` — keep scanning until EOF

    if not centres:
        raise ValueError(f"No cone centres found in {path}")
    return centres


def events_to_xy(df: pd.DataFrame,
                 centres: dict[int, tuple[float, float]]
                 ) -> tuple[np.ndarray, np.ndarray]:
    """
    Convert each (cone_id, ρ) pair to an (x, y) point for visualisation.
    Because φ is unknown, we displace every event purely along +x.
    """
    x_list, y_list = [], []
    for cid, rho in zip(df["cone_id"].values, df["rho_local[m]"].values):
        cx, cy = centres.get(cid, (np.nan, np.nan))
        x_list.append(cx + rho)   # +x offset only
        y_list.append(cy)
    return np.asarray(x_list), np.asarray(y_list)


# ----------------------------------------------------------------------------- #
#                               Data loading helper                             #
# ----------------------------------------------------------------------------- #
def read_tsv(path: Path) -> pd.DataFrame:
	"""
	Read the TSV while
		• skipping every line that starts with '#'
		• assigning explicit column names
	"""
	df = pd.read_csv(
		path,
		sep="\t",
		comment="#",                 # throw away the big dictionary header
		names=["cone_id", "rho_local[m]", "z_local[m]"],
		skip_blank_lines=True,
		engine="python",             # silences the regex-separator warning
	)

	# ------------------------------------------------------------------ #
	# 1) Convert each column to numeric, coercing errors to NaN
	df["cone_id"]      = pd.to_numeric(df["cone_id"],      errors="coerce")
	df["rho_local[m]"] = pd.to_numeric(df["rho_local[m]"], errors="coerce")
	df["z_local[m]"]   = pd.to_numeric(df["z_local[m]"],   errors="coerce")

	# 2) Toss any row where *any* of the three columns is NaN
	df = df.dropna(subset=["cone_id", "rho_local[m]", "z_local[m]"])

	# 3) Now every entry is numeric → safely cast cone_id to plain int
	df["cone_id"] = df["cone_id"].astype(int)
	# ------------------------------------------------------------------ #

	return df


# ----------------------------------------------------------------------------- #
#                              Plot-making helper                               #
# ----------------------------------------------------------------------------- #
def make_plots(df: pd.DataFrame,
               nbins: int,
               data_path: Path,          # <— NEW
               outdir: Path | None = None) -> None:
    rho = df["rho_local[m]"].values
    z   = df["z_local[m]"].values
    cones = df["cone_id"].values

    # --- Histogram of rho ------------------------------------------------------
    plt.figure(figsize=(6, 4))
    plt.hist(rho, bins=nbins, edgecolor="black", alpha=0.75)
    plt.xlabel("ρ  [m]")
    plt.ylabel("Ionization events")
    plt.title(f"Histogram of ρ   (bins = {nbins})")
    plt.tight_layout()
    if outdir:
        plt.savefig(outdir / "hist_rho.png", dpi=300)
        plt.close()

    # --- Histogram of z --------------------------------------------------------
    plt.figure(figsize=(6, 4))
    plt.hist(z, bins=nbins, edgecolor="black", alpha=0.75)
    plt.xlabel("z  [m]")
    plt.xlim(9e-7, z.max())  # set x-limits to data range
    plt.ylabel("Ionization events")
    plt.title(f"Histogram of z   (bins = {nbins})")
    plt.tight_layout()
    if outdir:
        plt.savefig(outdir / "hist_z.png", dpi=300)
        plt.close()

    # --- Histogram of cone_id --------------------------------------------------
    # Use one bin per integer cone_id so bars align with actual IDs
    min_id, max_id = cones.min(), cones.max()
    bins_cone = np.arange(min_id, max_id + 2) - 0.5   # centered bins

    plt.figure(figsize=(10, 4))
    plt.hist(cones, bins=bins_cone, edgecolor="black", alpha=0.85)
    plt.xlabel("Cone ID")
    plt.ylabel("Ionization events")
    
    plt.title("Ionization count per cone")
    plt.xticks(           # show only a subset of ticks to keep it readable
        ticks=np.linspace(min_id, max_id, 11, dtype=int),
        rotation=45,
        ha="right",
    )
    plt.tight_layout()
    if outdir:
        plt.savefig(outdir / "hist_cone_id.png", dpi=300)
        plt.close()

	# === NEW PLOT – 2-D map of the cones ====================================
    centres = parse_cone_centers(data_path)     
    cx_all, cy_all = zip(*centres.values())

    #  count events per cone for colours
    cone_counts = df["cone_id"].value_counts().reindex(centres.keys(),
                                                       fill_value=0).sort_index()

    plt.figure(figsize=(10, 5))
    sc = plt.scatter(
        cx_all, cy_all,
        c=cone_counts.values,
        cmap="viridis",
        s=80,
        edgecolors="black",
    )
    plt.colorbar(sc, label="Ionization events")

    # ---- optional: overlay every individual event as small grey dots -------
    ex, ey = events_to_xy(df, centres)
    plt.scatter(ex, ey, s=6, alpha=0.4, color="darkgrey")

    plt.xlabel("x  [m]")
    plt.ylabel("y  [m]")
    plt.title("Ionizations per cone (colour) + individual events (grey)")
    plt.gca().set_aspect("equal", adjustable="box")
    plt.tight_layout()
    plt.ylim(-4e-7, 4e-7) # set y-limits to data range

    if outdir:
        plt.savefig(outdir / "map_cones.png", dpi=300)
        plt.close()
        
    # If no --outdir was supplied, pop the figures up
    if not outdir:
        plt.show()


# ----------------------------------------------------------------------------- #
#                                   main()                                      #
# ----------------------------------------------------------------------------- #
def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file",   default="ionization_events.tsv")
    parser.add_argument("-n", "--nbins",  type=int, default=50)
    parser.add_argument("-o", "--outdir", type=Path, help="Save PNGs to this directory")
    args = parser.parse_args()

    df = read_tsv(Path(args.file))
    if args.outdir:
        args.outdir.mkdir(parents=True, exist_ok=True)
        make_plots(df,
               nbins=args.nbins,
               data_path=Path(args.file),   # <— NEW
               outdir=args.outdir)


if __name__ == "__main__":
    main()