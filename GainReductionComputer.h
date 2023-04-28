#pragma once

class GainReductionComputer {
public:
  GainReductionComputer(double threshold, double ratio, double kneeWidth)
    : threshold(threshold), ratio(ratio), kneeWidth(kneeWidth)
  {
    CalculateKneeLimits();
    CalculateReductionFactor();
  };

  double Compute(double);

  void SetThreshold(double threshold);
  void SetRatio(double ratio);
  void SetKneeWidth(double kneeWidth);

private:
  double threshold;
  double ratio;
  double kneeWidth;

  double kneeStart;
  double kneeEnd;
  double reductionFactor;

  void CalculateKneeLimits();
  void CalculateReductionFactor();
};


