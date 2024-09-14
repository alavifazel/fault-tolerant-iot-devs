#ifndef GENERATOR_N_SHOTS_BOOL_HPP
#define GENERATOR_N_SHOTS_BOOL_HPP

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

    enum class GeneratorNShotsBoolPhase
    {
        IDLE,
        GENERATING
    };

    struct GeneratorNShotsBoolState
    {
        double sigma;
        GeneratorNShotsBoolPhase phase;
        int numberOfFiresRemaining;
        GeneratorNShotsBoolState(GeneratorNShotsBoolPhase phase, int numberOfFires) : 
                sigma(0), phase(phase), numberOfFiresRemaining(numberOfFires) {}
    };

    std::ostream &operator<<(std::ostream &out, const GeneratorNShotsBoolState &s)
    {
        return out;
    }

    class GeneratorNShotsBool : public Atomic<GeneratorNShotsBoolState>
    {
    public:
        Port<bool> inStop;
        Port<bool> outData;

        std::function<double()> f_intervalGeneration;

        GeneratorNShotsBool(const std::string &id,
                  std::function<double()> f_intervalGeneration,
                  int numberOfFires) 
            :
                Atomic<GeneratorNShotsBoolState>(id, GeneratorNShotsBoolState(GeneratorNShotsBoolPhase::IDLE, numberOfFires)),
                f_intervalGeneration(f_intervalGeneration)

        {
            inStop = addInPort<bool>("inStop");
            outData = addOutPort<bool>("outData");
        }

        void internalTransition(GeneratorNShotsBoolState &s) const override
        {
            s.numberOfFiresRemaining--;
            if(s.numberOfFiresRemaining >= 0) {
                s.sigma = f_intervalGeneration();
                s.phase = GeneratorNShotsBoolPhase::GENERATING;
            } else {
				s.sigma = std::numeric_limits<double>::infinity();
				s.phase = GeneratorNShotsBoolPhase::IDLE;
            }
        }

        void externalTransition(GeneratorNShotsBoolState &s, double e) const override
        {
            if (!inStop->empty()) {
				s.sigma = std::numeric_limits<double>::infinity();
				s.phase = GeneratorNShotsBoolPhase::IDLE;
			}
        }

        void output(const GeneratorNShotsBoolState &s) const override {
            if(s.phase == GeneratorNShotsBoolPhase::GENERATING)
            {
                log("<GENERATOR_NSHOTS> Remaining number of fires: " + std::to_string(s.numberOfFiresRemaining));
                outData->addMessage(true);
            }
        }

        [[nodiscard]] double timeAdvance(const GeneratorNShotsBoolState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
