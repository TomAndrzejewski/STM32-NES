#!/usr/bin/env python3
"""Plot battery voltage against time from a SWO log.

Each log line holds tab-separated integers, the last one being VBAT in mV.
The chart is saved as a PNG next to the log file.

Usage: ./plot_vbat.py Log/swo_VBAT.log [--period 1.0] [--show]
"""

import argparse
from pathlib import Path

import matplotlib.pyplot as plt

LINE_COLOR = "#2a78d6"
TEXT_COLOR = "#52514e"
GRID_COLOR = "#e4e3df"
SECONDS_PER_HOUR = 3600


def read_vbat(path, period_s):
    """Return record times in hours, voltages in mV and the failed reading count."""
    hours = []
    voltages = []
    failed = 0

    with open(path, encoding="ascii") as log:
        for index, line in enumerate(log):
            voltage = int(line.split()[-1])

            # Zero marks a failed reading, not a real voltage
            if voltage == 0:
                failed += 1
                continue

            hours.append(index * period_s / SECONDS_PER_HOUR)
            voltages.append(voltage)

    return hours, voltages, failed


def plot_vbat(hours, voltages, title):
    """Draw the voltage line and return the figure."""
    fig, ax = plt.subplots(figsize=(11, 5), layout="constrained")

    ax.plot(hours, voltages, color=LINE_COLOR, linewidth=1.5)
    ax.set_title(title, loc="left", color=TEXT_COLOR)
    ax.set_xlabel("Time [h]", color=TEXT_COLOR)
    ax.set_ylabel("VBAT [mV]", color=TEXT_COLOR)
    ax.set_xlim(left=0)

    # Keep the axes recessive so the data line stands out
    ax.grid(color=GRID_COLOR, linewidth=0.8)
    ax.set_axisbelow(True)
    ax.spines[["top", "right", "left"]].set_visible(False)
    ax.tick_params(colors=TEXT_COLOR, length=0)

    return fig


def main():
    parser = argparse.ArgumentParser(description="Plot VBAT against time from a SWO log.")
    parser.add_argument("log", type=Path, help="path to the SWO log file")
    parser.add_argument("--period", type=float, default=1.0, help="seconds between records")
    parser.add_argument("--show", action="store_true", help="also open an interactive window")
    args = parser.parse_args()

    hours, voltages, failed = read_vbat(args.log, args.period)

    fig = plot_vbat(hours, voltages, args.log.name)
    output = args.log.with_suffix(".png")
    fig.savefig(output, dpi=150)

    print(f"Records plotted: {len(voltages)}, failed readings skipped: {failed}")
    print(f"VBAT: {voltages[0]} mV -> {voltages[-1]} mV, duration {hours[-1]:.2f} h")
    print(f"Saved: {output}")

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
