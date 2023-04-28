#include "GainReductionComputer.h"


double GainReductionComputer::Compute(double sample) {

  double gainReduction = 0.;

  if (sample <= kneeStart) {
    gainReduction = 0;                             // No reduction
  }
  else if(sample < kneeEnd) {
    // Quadratic interpolation for gain reduction
    double factor = (sample - kneeStart) / kneeWidth;
    factor *= factor;
    double delta = sample - kneeStart;
    gainReduction = factor * delta * reductionFactor;   // Gain Reduction in dBs
  }
  else {
    // Normal gain reduction
    double delta = sample - threshold;
    gainReduction = delta * reductionFactor;   // Gain Reduction in dBs
  }

  return gainReduction;
}

void GainReductionComputer::SetThreshold(double threshold)
{
  GainReductionComputer::threshold = threshold;
  CalculateKneeLimits();
}

void GainReductionComputer::SetRatio(double ratio)
{
  GainReductionComputer::ratio = ratio;
  CalculateReductionFactor();
}

void GainReductionComputer::SetKneeWidth(double kneeWidth)
{
  GainReductionComputer::kneeWidth = kneeWidth;
  CalculateKneeLimits();
}

void GainReductionComputer::CalculateKneeLimits()
{
  kneeStart = threshold - kneeWidth / 2.;
  kneeEnd = threshold + kneeWidth / 2.;
}

void GainReductionComputer::CalculateReductionFactor()
{
  reductionFactor = (1. - ratio) / ratio;
}
