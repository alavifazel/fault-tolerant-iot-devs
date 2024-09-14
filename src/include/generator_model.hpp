#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#ifndef SIMULATION
#include "cadmium/modeling/devs/atomic.hpp"
#else
#include <cadmium/core/modeling/atomic.hpp>
#endif

#include "device/message.hpp"
#include <iostream>
#include <string>
#include <random>
#include <memory>
#include <functional>
#include <random>
#include <memory>
#include <functional>

enum GeneratorPhase
{
    IDLE,
    GENERATING
};

namespace cadmium::iot
{
    struct GeneratorState
    {
        double clock;
        double sigma;
        GeneratorPhase phase;
        GeneratorState(GeneratorPhase phase) : clock(), sigma(), phase(phase) {}
    };

    std::ostream &operator<<(std::ostream &out, const GeneratorState &s)
    {
        return out;
    }

    class Generator : public Atomic<GeneratorState>
    {
    public:
        Port<bool> inStop;
        Port<double> outData;

        std::function<double()> f_intervalGeneration;
        std::function<double()> f_dataGeneration;

        Generator(const std::string &id,
                  std::function<double()> f_intervalGeneration,
                  std::function<double()> f_dataGeneration)
            :
                Atomic<GeneratorState>(id, GeneratorState(GeneratorPhase::IDLE)),
                f_intervalGeneration(f_intervalGeneration),
                f_dataGeneration(f_dataGeneration)

        {
            inStop = addInPort<bool>("inStop");
            outData = addOutPort<double>("outData");
        }

        void internalTransition(GeneratorState &s) const override
        {
            s.sigma = f_intervalGeneration();
            s.phase = GENERATING;
        }

        void externalTransition(GeneratorState &s, double e) const override
        {
            if (!inStop->empty()) {
				s.sigma = std::numeric_limits<double>::infinity();
				s.phase = GeneratorPhase::IDLE;
			}
        }

        void output(const GeneratorState &s) const override {
            outData->addMessage(f_dataGeneration());
        }

        [[nodiscard]] double timeAdvance(const GeneratorState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
