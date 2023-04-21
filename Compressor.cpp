#include "Compressor.h"
#include "Conversions.h"
#include <vector>

Compressor::Compressor(double threshold, double ratio, double kneeWidth, double sampleRate, double attackMs, double releaseMs)
  : threshold {threshold}
  , ratio {ratio}
  , kneeWidth{ kneeWidth }
  , grDetector{ sampleRate, attackMs, releaseMs }
{
}


void Compressor::ProcessBlock(double* input, double* sidechain, double* output, double* grMeter, int nFrames)
{
  std::vector<double> localBuffer(nFrames);

  // Log of sidechain signal
  for (int s = 0; s < nFrames; s++) {
    auto logSample = AmpToDB(input[s]);
    localBuffer[s] = logSample;
  }

  // Pass log of sidechain signal through gain curve
  // Here we have the sidechain signal converted to dBs between -infinite and zero (or greater)
  for (int s = 0; s < nFrames; s++) {
    double sample = localBuffer[s];

    if (sample > threshold)
    {
      double delta = sample - threshold;
      grMeter[s] = -(delta * (1. - 1. / ratio));   // Gain Reduction in dBs
    }
    else
    {
      grMeter[s] = 0;                             // No reduction
    }
  }

  // Back to linear for attacking the detector
  for (int s = 0; s < nFrames; s++) {
    grMeter[s] = DBToAmp(grMeter[s]);
  }

  // Attack/Release post gain curve
  for (int s = 0; s < nFrames; s++) {
    // Here we have a gain factor between 0dB and -inf, so we need to invert the input to the detector and its output.
    grMeter[s] = 1. - grDetector.ProcessSample(1. - grMeter[s]);
  }

  // Apply the gain profile
  for (int s = 0; s < nFrames; s++) {
    output[s] = input[s] * grMeter[s];
  }

}

void Compressor::SetSampleRate(double sampleRate)
{
  grDetector.setSampleRate(sampleRate);
}

void Compressor::SetAttack(double attackMs)
{
  grDetector.setAttack(attackMs);
}

void Compressor::SetRelease(double releaseMs)
{
  grDetector.setRelease(releaseMs);
}

void Compressor::SetThreshold(double threshold)
{
  Compressor::threshold = threshold;
}

void Compressor::SetRatio(double ratio)
{
  Compressor::ratio = ratio;
}

void Compressor::SetKneeWidth(double kneeWidth)
{
  Compressor::kneeWidth = kneeWidth;
}
