#ifndef RECEIVING_BUFFER_HPP
#define RECEIVING_BUFFER_HPP

#include <cadmium/core/modeling/atomic.hpp>
#include <iostream>
#include <queue>
#include <random>
#include "../mqtt_message.hpp"
#include "../mqtt_packet.hpp"
#include "../mqtt_client_command.hpp"
#include "buffer_packet.hpp"
#include "broker_message.hpp"
#include "utility.hpp"

namespace cadmium::iot
{
    using std::vector;

    enum class ReceivingBufferPhase
    {
        ACTIVE,
        PASSIVE
    };

    struct ReceivingBufferState
    {
        double sigma;
        ReceivingBufferPhase phase;
        std::queue<BufferPacket> buffer;
        ReceivingBufferState()
            : sigma(std::numeric_limits<double>::infinity()), phase(ReceivingBufferPhase::PASSIVE) {}
    };

    std::ostream &operator<<(std::ostream &out, const ReceivingBufferState &s)
    {
        out << s.sigma;
        return out;
    }

    class ReceivingBuffer : public Atomic<ReceivingBufferState>
    {
    private:
        double processingTimeExpMean;
        int numOfIOPorts;
        double generateProcessingTime() const
        {
            std::random_device rd;
            std::exponential_distribution<> rng(1 / processingTimeExpMean);
            std::mt19937 rnd_gen(rd());
            return rng(rnd_gen);
        }

    public:
        vector<Port<MQTTPacket>> in;
        Port<BrokerInternalMessage> out;

        ReceivingBuffer(const std::string &id, double processingTimeExpMean, int numOfIOPorts)
            : Atomic<ReceivingBufferState>(id, ReceivingBufferState()),
              numOfIOPorts(numOfIOPorts),
              processingTimeExpMean(processingTimeExpMean),
              in(numOfIOPorts)

        {
            for (int i = 0; i < numOfIOPorts; i++)
            {
                in[i] = addInPort<MQTTPacket>("in" + std::to_string(i));
            }
            out = addOutPort<BrokerInternalMessage>("out");
        }

        void internalTransition(ReceivingBufferState &s) const override
        {
            if(!s.buffer.empty())
            {
                s.buffer.pop();
                if (s.buffer.empty())
                {
                    // s.phase = ReceivingBufferPhase::PASSIVE;
                    s.sigma = std::numeric_limits<double>::infinity();
                }
                else
                {
                    s.sigma = generateProcessingTime();
                }
            } else
            {
                s.sigma = std::numeric_limits<double>::infinity();
            }

        }

        void externalTransition(ReceivingBufferState &s, double e) const override
        {
            for (size_t i = 0; i < numOfIOPorts; ++i)
            {
                if (!in[i]->empty())
                {
                    // log("<BUFFER> in on port :" + std::to_string(i));
                    auto newPacket = in[i]->getBag().back();
                    if (std::stoi(newPacket.getBody()["packetID"]) % 2 == 0
                     )
                    {
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::bernoulli_distribution d(1);
                        if(d(gen))
                        {
                            // cout << "XXXXXXXXXX" << endl;
                            s.buffer.push(BufferPacket(newPacket, i));
                        }
                        else
                        {
                            // cout << "ZZZZZZ" << endl;
                        }
                    }
                    else
                    {
                        s.buffer.push(BufferPacket(newPacket, i));
                    }
                }
            }
            s.sigma = generateProcessingTime();

            // switch (s.phase)
            // {
            // case ReceivingBufferPhase::ACTIVE:
            // {
            //     // s.sigma -= e;
            //     break;
            // }
            // case ReceivingBufferPhase::PASSIVE:
            // {
            //     s.sigma = generateProcessingTime();
            //     s.phase = ReceivingBufferPhase::ACTIVE;
            //     break;
            // }
            // }
        }

        void output(const ReceivingBufferState &s) const override
        {
            log("<BUFFER> out.");
            if(!s.buffer.empty())
            {
                BufferPacket bufferPacket = s.buffer.front();
                out->addMessage(BrokerInternalMessage(bufferPacket.portNumber, bufferPacket.packet));
            }

        }

        [[nodiscard]] double timeAdvance(const ReceivingBufferState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
