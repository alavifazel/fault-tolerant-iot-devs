#ifndef WRAPPER_HPP
#define WRAPPER_HPP

#ifndef SIMULATION
#include "cadmium/modeling/devs/atomic.hpp"
#else
#include <cadmium/core/modeling/atomic.hpp>
#endif

#include "message.hpp"

#include <iostream>
#include <string>
#include <random>
#include <memory>
#include <functional>
#include "../utility.hpp"

enum class WrapperPhase
{
    IDLE,
    DATA_AVAILABLE
};

namespace cadmium::iot
{
    struct WrapperState
    {
        double data;
        double sigma;
        WrapperPhase phase;
        WrapperState(WrapperPhase phase, double sigma = std::numeric_limits<double>::max())
            : sigma(sigma), phase(phase) {}
    };

    std::ostream &operator<<(std::ostream &out, const WrapperState &s)
    {
        out << s.data;
        return out;
    }

    class Wrapper : public Atomic<WrapperState>
    {
    public:
        Port<double> inData;
        Port<Message<double>> outData;

        std::string originator;
        std::string deviceType;

        Wrapper(const std::string &id, std::string originator, std::string deviceType)
            :
                Atomic<WrapperState>(id, WrapperState(WrapperPhase::IDLE)),
                originator(originator),
                deviceType(deviceType)

        {
            inData = addInPort<double>("inData");
            outData = addOutPort<Message<double>>("outData");
        }

        void internalTransition(WrapperState &s) const override
        {
            s.sigma = std::numeric_limits<double>::infinity();
			s.phase = WrapperPhase::IDLE;
        }

        void externalTransition(WrapperState &s, double e) const override
        {
            if (!inData->empty()) {
				s.sigma = 0;
                s.data = inData->getBag().back();
				s.phase = WrapperPhase::DATA_AVAILABLE;
                log("<WRAPPER> Received " + std::to_string(s.data));
			}
        }

        void output(const WrapperState &s) const override {
            if(s.phase == WrapperPhase::DATA_AVAILABLE) {
                outData->addMessage(Message<double>(originator, deviceType, s.data));
            }
        }

        [[nodiscard]] double timeAdvance(const WrapperState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
