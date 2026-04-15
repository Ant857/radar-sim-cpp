#include "matched_filter.h"
#include <fftw3.h>
#include <cmath>
#include <cstring>

// Next power of 2, made to be equivalent to Octave's 2^nextpow2
static int next_pow2(int n) {
    int p = 1;
    while (p < n) p <<= 1;
    return p;
}

Eigen::MatrixXcd matched_filter(
    const Eigen::MatrixXcd& rx_signal,
    const std::vector<std::complex<double>>& pulse,
    const RadarParams& params)
{
    int spp = params.samples_per_pulse;
    int num_pulses = params.num_pulses;
    int pulse_len = static_cast<int>(pulse.size());
    int nfft = next_pow2(spp + pulse_len - 1);

    Eigen::MatrixXcd compressed(spp, num_pulses);

    // Allocate FFTW buffers
    fftw_complex* in  = fftw_alloc_complex(nfft);
    fftw_complex* out = fftw_alloc_complex(nfft);
    fftw_complex* ref_freq = fftw_alloc_complex(nfft);
    fftw_complex* prod = fftw_alloc_complex(nfft);

    // Create plans
    fftw_plan fwd  = fftw_plan_dft_1d(nfft, in, out, FFTW_FORWARD,  FFTW_ESTIMATE);
    fftw_plan inv  = fftw_plan_dft_1d(nfft, prod, out, FFTW_BACKWARD, FFTW_ESTIMATE);

    // Compute reference spectrum
    // This should be equivalent to Octave's conj(fft(pulse, nfft))
    std::memset(in, 0, nfft * sizeof(fftw_complex));
    for (int i = 0; i < pulse_len; ++i) {
        in[i][0] = pulse[i].real();
        in[i][1] = pulse[i].imag();
    }
    fftw_execute_dft(fwd, in, ref_freq);

    // Conjugate the reference spectrum
    for (int i = 0; i < nfft; ++i) {
        ref_freq[i][1] = -ref_freq[i][1];
    }

    // Process each pulse column
    for (int j = 0; j < num_pulses; ++j) {
        // Load received signal column into buffer
        std::memset(in, 0, nfft * sizeof(fftw_complex));
        for (int i = 0; i < spp; ++i) {
            in[i][0] = rx_signal(i, j).real();
            in[i][1] = rx_signal(i, j).imag();
        }

        // FFT of received signal
        fftw_execute_dft(fwd, in, out);

        // Multiply with reference spectrum in frequency domain
        for (int i = 0; i < nfft; ++i) {
            prod[i][0] = out[i][0] * ref_freq[i][0] - out[i][1] * ref_freq[i][1];
            prod[i][1] = out[i][0] * ref_freq[i][1] + out[i][1] * ref_freq[i][0];
        }

        // IFFT back to time domain
        fftw_execute_dft(inv, prod, out);

        // Copy result 
        // Dividing by nfft, since FFTW doesn't normalize
        for (int i = 0; i < spp; ++i) {
            compressed(i, j) = std::complex<double>(
                out[i][0] / nfft,
                out[i][1] / nfft
            );
        }
    }

    // Clean up
    fftw_destroy_plan(fwd);
    fftw_destroy_plan(inv);
    fftw_free(in);
    fftw_free(out);
    fftw_free(ref_freq);
    fftw_free(prod);

    return compressed;
}
