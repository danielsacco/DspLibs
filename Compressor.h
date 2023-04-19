#pragma once

#include <../DspLibs/Detectors.h>

class Compressor {
public:
  Compressor(double threshold, double ratio, double kneeWidth, double sampleRate, double attackMs, double releaseMs);

  void ProcessBlock(double* input, double* sidechain, double* output, double* grMeter, double* scMeter, double* outMeter, int nFrames);

  void SetSampleRate(double sampleRate);
  void SetAttack(double attackMs);
  void SetRelease(double releaseMs);

  void SetThreshold(double threshold);
  void SetRatio(double ratio);
  void SetKneeWidth(double kneeWidth);

private:
  double threshold;
  double ratio;
  double kneeWidth;

  DecoupledPeakDetector grDetector;
  DecoupledPeakDetector scDetector;
  DecoupledPeakDetector outDetector;

};
