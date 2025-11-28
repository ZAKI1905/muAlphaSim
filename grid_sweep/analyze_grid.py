#!/usr/bin/env python3

import os
import glob
import json
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def collect_run_data(base_path="."):
    """
    Walk through directories named "nx{nx}_ny{ny}" (nx, ny = 1..5), find the
    run_*.json file inside each "results/" subdirectory, and return a DataFrame
    with the relevant fields pulled from each JSON. Also computes propagated error
    for ionization-to-capture ratio.
    """
    records = []

    for nx in range(1, 6):
        for ny in range(1, 6):
            dir_name = f"nx{nx}_ny{ny}"
            results_folder = os.path.join(base_path, dir_name, "results")
            
            # Look for any file matching run_*.json
            json_pattern = os.path.join(results_folder, "run_*.json")
            json_files = glob.glob(json_pattern)
            if not json_files:
                print(f"Warning: No JSON found in '{results_folder}'")
                continue

            # If there are multiple JSONs, pick the one with latest modification time
            latest_json = max(json_files, key=os.path.getmtime)

            try:
                with open(latest_json, "r") as jf:
                    data = json.load(jf)
            except Exception as e:
                print(f"Error reading '{latest_json}': {e}")
                continue

            # Extract values and errors
            ion_frac = data.get("ion_frac", None)
            ion_err  = data.get("ion_err",  None)
            cap_frac = data.get("cap_frac", None)
            cap_err  = data.get("cap_err",  None)
            ion_cap_ratio = data.get("ion_cap_ratio", None)
            
            # Propagate error for R = I / C: σ_R = R * sqrt((σ_I/I)^2 + (σ_C/C)^2)
            if ion_frac is not None and cap_frac is not None and ion_err is not None and cap_err is not None:
                if ion_frac > 0 and cap_frac > 0:
                    ratio_err = ion_cap_ratio * np.sqrt((ion_err/ion_frac)**2 + (cap_err/cap_frac)**2)
                else:
                    ratio_err = None
            else:
                ratio_err = None

            # Build a record from the JSON fields we care about
            record = {
                "nx": nx,
                "ny": ny,
                "n_events":       data.get("n_events",       None),
                "n_ion":          data.get("n_ion",          None),
                "n_capture":      data.get("n_capture",      None),
                "ion_frac":       ion_frac,
                "ion_err":        ion_err,
                "cap_frac":       cap_frac,
                "cap_err":        cap_err,
                "ion_cap_ratio":  ion_cap_ratio,
                "ion_cap_err":    ratio_err
            }
            records.append(record)

    # Convert to DataFrame
    df = pd.DataFrame.from_records(records)
    return df

def rank_and_print(df, column_name, top_n=5):
    """
    Sort the DataFrame 'df' by 'column_name' (descending) and print the top_n rows.
    """
    if column_name not in df.columns:
        print(f"Column '{column_name}' not found.")
        return

    sorted_df = df.sort_values(by=column_name, ascending=False).reset_index(drop=True)
    print(f"\nTop {top_n} geometries ranked by '{column_name}':")
    print(sorted_df.loc[:, ["nx", "ny", column_name]].head(top_n).to_string(index=False))

def plot_ion_frac_vs_ny(df):
    """
    For each fixed nx (1..5), plot ion_frac vs. ny (1..5) with error bars.
    Each nx is a separate curve (circle markers, solid lines).
    """
    plt.figure(figsize=(8, 6))
    for nx_value in sorted(df["nx"].unique()):
        subset = df[df["nx"] == nx_value].copy().sort_values(by="ny")
        xs = subset["ny"]
        ys = subset["ion_frac"]
        yerr = subset["ion_err"]
        plt.errorbar(xs, ys, yerr=yerr, marker='o', linestyle='-', capsize=3, label=f"nx = {nx_value}")

    plt.xlabel("ny (number of columns)")
    plt.ylabel("Ionization Fraction")
    plt.title("Ionization Fraction vs. ny, grouped by nx")
    plt.legend(title="Fixed nx")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.xticks(range(1, 6))
    plt.tight_layout()

def plot_cap_frac_vs_ny(df):
    """
    For each fixed nx (1..5), plot cap_frac vs. ny (1..5) with error bars.
    Each nx is a separate curve (square markers, dashed lines).
    """
    plt.figure(figsize=(8, 6))
    for nx_value in sorted(df["nx"].unique()):
        subset = df[df["nx"] == nx_value].copy().sort_values(by="ny")
        xs = subset["ny"]
        ys = subset["cap_frac"]
        yerr = subset["cap_err"]
        plt.errorbar(xs, ys, yerr=yerr, marker='s', linestyle='--', capsize=3, label=f"nx = {nx_value}")

    plt.xlabel("ny (number of columns)")
    plt.ylabel("Capture Fraction")
    plt.title("Capture Fraction vs. ny, grouped by nx")
    plt.legend(title="Fixed nx")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.xticks(range(1, 6))
    plt.tight_layout()

def plot_ion_cap_ratio_vs_ny(df):
    """
    For each fixed nx (1..5), plot ion_cap_ratio vs. ny (1..5) with propagated error bars.
    Each nx is a separate curve (triangle markers, dash-dot lines).
    """
    plt.figure(figsize=(8, 6))
    for nx_value in sorted(df["nx"].unique()):
        subset = df[df["nx"] == nx_value].copy().sort_values(by="ny")
        xs = subset["ny"]
        ys = subset["ion_cap_ratio"]
        yerr = subset["ion_cap_err"]
        plt.errorbar(xs, ys, yerr=yerr, marker='^', linestyle='-.', capsize=3, label=f"nx = {nx_value}")

    plt.xlabel("ny (number of columns)")
    plt.ylabel("Ionization/Capture Ratio")
    plt.title("Ionization-to-Capture Ratio vs. ny, grouped by nx")
    plt.legend(title="Fixed nx")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.xticks(range(1, 6))
    plt.tight_layout()

if __name__ == "__main__":
	# 1) Collect all run data into a DataFrame
	df_runs = collect_run_data(base_path=".")

    # 2) If no data, stop
	if df_runs.empty:
		print("No run data collected. Exiting.")
	else:
		# 3) Rank by n_ion
		rank_and_print(df_runs, "n_ion", top_n=5)

		# 4) Rank by n_capture
		rank_and_print(df_runs, "n_capture", top_n=5)

		# 5) Rank by ion_cap_ratio
		rank_and_print(df_runs, "ion_cap_ratio", top_n=5)

		# 6) Plot ionization fraction vs. ny for each fixed nx
		plot_ion_frac_vs_ny(df_runs)
		# plt.show()
		plt.savefig("ionization_fraction_vs_ny.png")

		# 7) Plot capture fraction vs. ny for each fixed nx
		plot_cap_frac_vs_ny(df_runs)
		# plt.show()
		plt.savefig("capture_fraction_vs_ny.png")

		# 8) Plot ionization-to-capture ratio vs. ny for each fixed nx
		plot_ion_cap_ratio_vs_ny(df_runs)
		# plt.show()
		plt.savefig("ionization_capture_ratio_vs_ny.png")

		# 9) (Optional) Save to CSV
		df_runs.to_csv("all_runs_summary.csv", index=False)
		df_runs.sort_values(by="n_ion", ascending=False)\
				.to_csv("rank_by_n_ion.csv", index=False)
		df_runs.sort_values(by="n_capture", ascending=False)\
				.to_csv("rank_by_n_capture.csv", index=False)
		df_runs.sort_values(by="ion_cap_ratio", ascending=False)\
				.to_csv("rank_by_ion_cap_ratio.csv", index=False)