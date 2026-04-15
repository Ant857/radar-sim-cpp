#ifndef RADAR_PARAMS_H
#define RADAR_PARAMS_H

#include <vector>
#include <cmath>

struct Target {
    double range_m;
    double velocity_mps;
    double rcs_m2;
};

struct RadarParams {

    // Physical constants
    static constexpr double c = 3e8;
    static constexpr double boltzmann = 1.38e-23;

    // Radar system parameters
    double fc           = 10e9;       // carrier frequency (Hz)
    double bw           = 5e6;        // chirp bandwidth (Hz)
    double pulse_width  = 10e-6;      // pulse duration (s)
    double prf          = 10e3;       // pulse repetition frequency (Hz)
    int    num_pulses   = 64;         // pulses per coherent processing interval
    double fs           = 2 * bw;     // sampling rate (Hz)
    double tx_power     = 1000.0;     // transmit power (W)
    double gain_dB      = 30.0;       // antenna gain (dB)
    double noise_fig_dB = 13.0;       // receiver noise figure (dB)
    double temp         = 290.0;      // system temperature (K)

    // CFAR parameters
    int    cfar_train   = 5;          // training cells per side
    int    cfar_guard   = 2;          // guard cells per side
    double cfar_alpha   = 100.0;      // detection threshold multiplier

    // Targets
    std::vector<Target> targets = {
        {5000.0,    50.0,  1.0},    // 5 km, 50 m/s approaching, 1 m^2 RCS
        {8000.0,  -120.0,  2.0},    // 8 km, 120 m/s receding, 2 m^2 RCS
        {12000.0,   30.0,  0.5},    // 12 km, 30 m/s approaching, 0.5 m^2 RCS
    };

    // Derived quantities computed in compute_derived()
    double lambda;
    double gain;
    double noise_figure;
    double noise_power;
    double range_res;
    double max_range;
    double max_velocity;
    double vel_res;
    int    samples_per_pulse;

    // Constructor computes all derived values
    RadarParams() { compute_derived(); }

    void compute_derived() {
        lambda            = c / fc;
        gain              = std::pow(10.0, gain_dB / 10.0);
        noise_figure      = std::pow(10.0, noise_fig_dB / 10.0);
        noise_power       = boltzmann * temp * bw * noise_figure;
        range_res         = c / (2.0 * bw);
        max_range         = c / (2.0 * prf);
        max_velocity      = lambda * prf / 4.0;
        vel_res           = lambda / (2.0 * num_pulses / prf);
        samples_per_pulse = static_cast<int>(std::round(fs / prf));
    }
};

#endif // RADAR_PARAMS_H
