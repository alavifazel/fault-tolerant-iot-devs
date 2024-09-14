#ifndef RANDOM_H_
#define RANDOM_H_

#include <random>

class Random {
public:
    virtual double getValue() = 0;
};


class UniformRealDistribution : public Random {
private:
    std::random_device rd;
    std::default_random_engine engine;
    std::uniform_real_distribution<double> distribution;
public:
    UniformRealDistribution(size_t min, size_t max) : rd(), engine(rd()), distribution(min, max)
    {}

    double getValue() {
        return distribution(engine);
    }
};

#endif
