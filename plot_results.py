import sys
import numpy as np
import matplotlib.pyplot as plt
import os


def main():
    data_dir = sys.argv[1] if len(sys.argv) > 1 else "."

    # Load data
    raw = np.loadtxt(os.path.join(data_dir, "raw_signal.csv"),
                     delimiter=",", skiprows=1)
    comp = np.loadtxt(os.path.join(data_dir, "compressed_signal.csv"),
                      delimiter=",", skiprows=1)
    det = np.loadtxt(os.path.join(data_dir, "detections.csv"),
                     delimiter=",", skiprows=1, ndmin=2)
    truth = np.loadtxt(os.path.join(data_dir, "ground_truth.csv"),
                       delimiter=",", skiprows=1)

    with open(os.path.join(data_dir, "rdm.csv")) as f:
        header = f.readline().strip().split(",")
    vel_axis = np.array([float(v) for v in header[1:]])

    rdm_data = np.loadtxt(os.path.join(data_dir, "rdm.csv"),
                          delimiter=",", skiprows=1)
    range_axis = rdm_data[:, 0]
    rdm_db = rdm_data[:, 1:]

    # Create figure
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle("Pulse-Doppler Radar Simulation", fontsize=14, fontweight="bold")

    # Raw received signal
    ax = axes[0, 0]
    ax.plot(raw[:, 0], raw[:, 1], linewidth=0.5)
    ax.set_xlabel("Range (km)")
    ax.set_ylabel("Amplitude")
    ax.set_title("Raw Received Signal — Pulse 1")
    ax.grid(True, alpha=0.3)

    # Signal matched filtering
    ax = axes[0, 1]
    ax.plot(comp[:, 0], comp[:, 1], linewidth=0.5)
    ax.set_xlabel("Range (km)")
    ax.set_ylabel("Amplitude")
    ax.set_title("After Matched Filtering — Pulse 1")
    ax.grid(True, alpha=0.3)

    # Range-Doppler Map
    ax = axes[1, 0]
    im = ax.pcolormesh(vel_axis, range_axis, rdm_db, shading="auto", cmap="jet")
    ax.set_xlabel("Velocity (m/s)")
    ax.set_ylabel("Range (km)")
    ax.set_title("Range-Doppler Map")
    fig.colorbar(im, ax=ax, label="dB")

    # RDM with CFAR detections
    ax = axes[1, 1]
    im = ax.pcolormesh(vel_axis, range_axis, rdm_db, shading="auto", cmap="jet")
    ax.set_xlabel("Velocity (m/s)")
    ax.set_ylabel("Range (km)")
    ax.set_title("CFAR Detections")
    fig.colorbar(im, ax=ax, label="dB")

    if det.size > 0:
        ax.plot(det[:, 1], det[:, 0], "ro", markersize=12,
                markerfacecolor="none", markeredgewidth=2, label="Detections")
        ax.legend()

    plt.tight_layout()
    out_path = os.path.join(data_dir, "radar_results.png")
    plt.savefig(out_path, dpi=150)
    print(f"Saved plot to {out_path}")
    plt.show()


if __name__ == "__main__":
    main()
