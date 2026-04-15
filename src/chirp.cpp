#include "chirp.h"
#include <cmath>

std::vector<std::complex<double>> generate_chirp(const RadarParams& params) {
    // Chirp rate
    double k = params.bw / params.pulse_width;

    // Number of samples in one pulse
    int num_samples = static_cast<int>(std::round(params.fs * params.pulse_width));

    std::vector<std::complex<double>> pulse(num_samples);

    for (int i = 0; i < num_samples; ++i) {
        double t = static_cast<double>(i) / params.fs;
        // LFM chirp: exp(j * pi * k * t^2)
        double phase = M_PI * k * t * t;
        pulse[i] = std::complex<double>(std::cos(phase), std::sin(phase));
    }

    return pulse;
}
