#include "Compressor.h"
#include "Conversions.h"

Compressor::Compressor(double threshold, double ratio, double kneeWidth, double sampleRate, double attackMs, double releaseMs)
  : threshold {threshold}
  , ratio {ratio}
  , kneeWidth{ kneeWidth }
  , grDetector{ sampleRate, attackMs, releaseMs }
  , scDetector{ sampleRate, attackMs, releaseMs }
  , outDetector{ sampleRate, attackMs, releaseMs }
{
}


void Compressor::ProcessBlock(double* input, double* sidechain, double* output, double* grMeter, double* scMeter, double* outMeter, int nFrames)
{
  // Log of sidechain signal, use scMeter as buffer
  for (int s = 0; s < nFrames; s++) {
    auto logSample = AmpToDB(input[s]);
    scMeter[s] = logSample;
  }

  // Pass log of sidechain signal through gain curve
  // Here we have the sidechain signal converted to dBs between -infinite and zero (or greater)
  for (int s = 0; s < nFrames; s++) {
    double sample = scMeter[s];

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

  // Fill sc and out meters
  for (int s = 0; s < nFrames; s++) {
    scMeter[s] = scDetector.ProcessSample(DBToAmp(scMeter[s]));
    outMeter[s] = outDetector.ProcessSample(output[s]);
  }
}

void Compressor::SetSampleRate(double sampleRate)
{
  grDetector.setSampleRate(sampleRate);
  scDetector.setSampleRate(sampleRate);
  outDetector.setSampleRate(sampleRate);
}

void Compressor::SetAttack(double attackMs)
{
  grDetector.setAttack(attackMs);
  scDetector.setAttack(attackMs);
  outDetector.setAttack(attackMs);
}

void Compressor::SetRelease(double releaseMs)
{
  grDetector.setRelease(releaseMs);
  scDetector.setRelease(releaseMs);
  outDetector.setRelease(releaseMs);
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
