#pragma once

class DistributionBase
{
public:
    virtual double Take() = 0;
};

class DistributionFix : public DistributionBase
{
protected:
    double Value;
public:
    DistributionFix(double val);
    double Take() override;
};

class DistributionUniform : public DistributionBase
{
protected:
    double Min, Max;
    std::uniform_real_distribution<double> Dist;
    std::shared_ptr<std::mt19937> engine;

public:
    DistributionUniform(std::shared_ptr<std::mt19937> mt, double min, double max);
    double Take() override;
};

class DistributionNormal : public DistributionBase
{
protected:
    double Mu, Chi;
    std::normal_distribution<double> Dist;
    std::shared_ptr<std::mt19937> engine;

public:
    DistributionNormal(std::shared_ptr<std::mt19937> mt, double mu, double chi2);
    double Take() override;
};
