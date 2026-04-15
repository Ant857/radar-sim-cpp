#include "simulate.h"
#include <cmath>
#include <random>

Eigen::MatrixXcd simulate_returns(
    const std::vector<std::complex<double>>& pulse,
    const RadarParams& params)
{
    int spp = params.samples_per_pulse;
    int num_pulses = params.num_pulses;
    int pulse_len = static_cast<int>(pulse.size());

    Eigen::MatrixXcd rx_signal = Eigen::MatrixXcd::Zero(spp, num_pulses);

    for (const auto& tgt : params.targets) {
        // Time delay from target range
        double tau = 2.0 * tgt.range_m / RadarParams::c;

        // Doppler frequency shift
        double fd = 2.0 * tgt.velocity_mps * params.fc / RadarParams::c;

        // Received power, this is our radar equation
        double Pr = (params.tx_power * params.gain * params.gain
                     * params.lambda * params.lambda * tgt.rcs_m2)
                    / (std::pow(4.0 * M_PI, 3) * std::pow(tgt.range_m, 4));
        double amplitude = std::sqrt(Pr);

        // Convert delay to sample index
        int delay_samples = static_cast<int>(std::round(tau * params.fs));

        // Place delayed chirp into each pulse column with Doppler phase shift
        for (int n = 0; n < num_pulses; ++n) {
            if (delay_samples + pulse_len - 1 < spp) {
                // Doppler phase shift for this pulse
                std::complex<double> doppler_shift(
                    std::cos(2.0 * M_PI * fd * n / params.prf),
                    std::sin(2.0 * M_PI * fd * n / params.prf)
                );

                for (int k = 0; k < pulse_len; ++k) {
                    rx_signal(delay_samples + k, n) +=
                        amplitude * pulse[k] * doppler_shift;
                }
            }
        }
    }

    // Add noise
    std::mt19937 gen(1);  // fixed seed, so reproducable
    std::normal_distribution<double> dist(0.0, std::sqrt(params.noise_power / 2.0));

    for (int j = 0; j < num_pulses; ++j) {
        for (int i = 0; i < spp; ++i) {
            rx_signal(i, j) += std::complex<double>(dist(gen), dist(gen));
        }
    }

    return rx_signal;
}
