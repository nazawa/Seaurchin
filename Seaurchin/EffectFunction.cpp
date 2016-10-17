#include "EffectFunction.h"

using namespace std;

DistributionFix::DistributionFix(double val)
{
    Value = val;
}

double DistributionFix::Take()
{
    return Value;
}

DistributionUniform::DistributionUniform(shared_ptr<mt19937> mt, double min, double max)
{
    engine = mt;
    Min = min;
    Max = max;
    Dist = uniform_real_distribution<double>(min, max);
}

double DistributionUniform::Take()
{
    return Dist(*engine);
}

DistributionNormal::DistributionNormal(shared_ptr<mt19937> mt, double mu, double chi2)
{
    engine = mt;
    Mu = mu;
    Chi = sqrt(chi2);
    Dist = normal_distribution<double>(Mu, Chi);
}

double DistributionNormal::Take()
{
    return Dist(*engine);
}
