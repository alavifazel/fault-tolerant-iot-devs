#ifndef GENERATOR_N_SHOTS_HPP_
#define GENERATOR_N_SHOTS_HPP_ 

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
#include "utility.hpp"

namespace cadmium::iot
{

    enum class GeneratorNShotsPhase
    {
        IDLE,
        GENERATING
    };

    struct GeneratorNShotsState
    {
        double clock;
        double sigma;
        GeneratorNShotsPhase phase;
        int numberOfFiresRemaining;
        GeneratorNShotsState(GeneratorNShotsPhase phase, int numberOfFires) : 
                clock(), sigma( 0 ), phase(phase), numberOfFiresRemaining(numberOfFires) {}
    };

    std::ostream &operator<<(std::ostream &out, const GeneratorNShotsState &s)
    {
        return out;
    }

    class GeneratorNShots : public Atomic<GeneratorNShotsState>
    {
    public:
        Port<bool> inStop;
        Port<double> outData;

        std::function<double()> f_intervalGeneration;
        std::function<double(std::string, int rem)> f_dataGeneration;

        GeneratorNShots(const std::string &id,
                  std::function<double()> f_intervalGeneration,
                  std::function<double(std::string, int rem)> f_dataGeneration,
                  int numberOfFires) 
            :
                Atomic<GeneratorNShotsState>(id, GeneratorNShotsState(GeneratorNShotsPhase::IDLE, numberOfFires)),
                f_intervalGeneration(f_intervalGeneration),
                f_dataGeneration(f_dataGeneration)

        {
            inStop = addInPort<bool>("inStop");
            outData = addOutPort<double>("outData");
        }

        void internalTransition(GeneratorNShotsState &s) const override
        {
            s.numberOfFiresRemaining--;
            if(s.numberOfFiresRemaining >= 0) {
                s.sigma = f_intervalGeneration();
                s.phase = GeneratorNShotsPhase::GENERATING;
            } else {
				s.sigma = std::numeric_limits<double>::infinity();
				s.phase = GeneratorNShotsPhase::IDLE;
            }
        }

        void externalTransition(GeneratorNShotsState &s, double e) const override
        {
            if (!inStop->empty())
            {
				s.sigma = std::numeric_limits<double>::infinity();
				s.phase = GeneratorNShotsPhase::IDLE;
			}
        }

        void output(const GeneratorNShotsState &s) const override {
            if(s.phase == GeneratorNShotsPhase::GENERATING)
            {
                auto output = f_dataGeneration(id, s.numberOfFiresRemaining);
                log("<GEN> Output data:" + std::to_string(output) + " Remaining #: " + std::to_string(s.numberOfFiresRemaining));
                outData->addMessage(output);
            }
        }

        [[nodiscard]] double timeAdvance(const GeneratorNShotsState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
