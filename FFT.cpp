
#ifndef __FFT__

#define __FFT__

#include <cmath>
#include <complex>
#include <valarray>

using namespace std;

namespace FFT {
  
  typedef complex<double> Complex;
  typedef valarray<Complex> CArray;

  void fft(CArray& x) {
    const size_t N = x.size();
    if(N <= 1) return;
    CArray even = x[slice(0, N/2, 2)];
    CArray odd = x[slice(1, N/2, 2)];

    fft(even);
    fft(odd);

    for(size_t k = 0;k < N/2;++k) {
      Complex t = polar(1.0, -2.0 * M_PI * k / N) * odd[k];
      x[k] = even[k] + t;
      x[k+N/2] = even[k] - t;
    }
  }
  
  void ifft(CArray& x) {
    x = x.apply(conj);
    fft(x);
    x = x.apply(conj);
    x /= x.size();
  }

}

#endif
