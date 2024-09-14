
#ifndef DAQ_HPP
#define DAQ_HPP

#ifndef SIMULATION
#include "cadmium/modeling/devs/atomic.hpp"
#else
#include <cadmium/core/modeling/atomic.hpp>
// #include "esp_log.h"
#endif

#include "../../utilities/brooks_iyengar.hpp"

#include <iostream>
#include <string>
#include "./message.hpp"
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace cadmium::iot
{
    struct DAQState
    {
        double sigma;
        vector<double> bufferedMessages;
        bool init;
        DAQState()
            : 
              sigma(std::numeric_limits<double>::max()),
              init(true)
            {
            }
    };

    std::ostream &operator<<(std::ostream &out, const DAQState &s)
    {
        out << s.bufferedMessages.size();
        return out;
    }

    class DAQ : public Atomic<DAQState>
    {
    private:
        double windowSize;
    public:
        Port<Message<double>> inData;
        Port<Message<double>> outData;

        DAQState &getState() { return state; }

        DAQ(const std::string &id, double windowSize = 2)
         : Atomic<DAQState>(id, DAQState()), windowSize(windowSize)
        {
			inData = addInPort<Message<double>>("inData");
            outData = addOutPort<Message<double>>("outData");
        }

        void internalTransition(DAQState &s) const override
        {
            if(s.bufferedMessages.size() == 0)
            {
                s.sigma = std::numeric_limits<double>::max();
                return;
            }
            s.sigma = windowSize;
            s.bufferedMessages.clear();
        }

        void externalTransition(DAQState &s, double e) const override
        {
            if(!inData->empty())
            {
                if(s.sigma == std::numeric_limits<double>::max())
                {
                    s.sigma = windowSize;
                }
                else 
                {
                    s.sigma -= e;
                }

                auto newMessage = inData->getBag().back();
                s.bufferedMessages.push_back(newMessage.getData());
            }
        }

        void output(const DAQState &s) const override {
            if(s.bufferedMessages.size() != 0)
            {
                vector<pair<double,double>> values;
                for(size_t i = 0; i < s.bufferedMessages.size(); ++i)
                {
                    values.push_back(make_pair(s.bufferedMessages[i]-1, s.bufferedMessages[i]+1));
                }
                outData->addMessage(Message<double>("DAQ","output", brooksIyengar(values,0)));
            }
        }

        [[nodiscard]] double timeAdvance(const DAQState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
