#ifndef SENDING_BUFFER_IN_HPP
#define SENDING_BUFFER_IN_HPP

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

    enum class SendingBufferPhase
    {
        ACTIVE,
        PASSIVE
    };

    struct SendingBufferState
    {
        double sigma;
        SendingBufferPhase phase;
        std::queue<BufferPacket> buffer;
        SendingBufferState()
         : sigma(std::numeric_limits<double>::infinity()), phase(SendingBufferPhase::PASSIVE) {}
    };

    std::ostream &operator<<(std::ostream &out, const SendingBufferState &s)
    {
        out << s.sigma;
        return out;
    }

    class SendingBuffer : public Atomic<SendingBufferState>
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
        Port<vector<BrokerInternalMessage>> in;
        vector<Port<MQTTPacket>> out;

        SendingBuffer(const std::string &id, double processingTimeExpMean, int numOfIOPorts)
            : Atomic<SendingBufferState>(id, SendingBufferState()),
              numOfIOPorts(numOfIOPorts),
              processingTimeExpMean(processingTimeExpMean),
              out(numOfIOPorts)

        {
            for (int i = 0; i < numOfIOPorts; i++)
            {
                out[i] = addOutPort<MQTTPacket>("out" + std::to_string(i));
            }
            in = addInPort<vector<BrokerInternalMessage>>("in");
        }

        void internalTransition(SendingBufferState &s) const override
        {
            if(!s.buffer.empty())
            {            
                s.buffer.pop();
                if(s.buffer.empty()) {
                    s.phase = SendingBufferPhase::PASSIVE;
                    s.sigma = std::numeric_limits<double>::infinity();
                } else {
                    s.sigma = generateProcessingTime();
                }
            }
            else
            {
                s.sigma = std::numeric_limits<double>::infinity();
            }
        }

        void externalTransition(SendingBufferState &s, double e) const override
        {

            if (!in->empty())
            {
                auto newPackets = in->getBag().back();

                log("Number of packets received in the sending buffer: " + std::to_string(newPackets.size()));
                for(int i = 0; i < newPackets.size(); i++) {
                    s.buffer.push(BufferPacket(newPackets[i].packet, newPackets[i].portNumber));
                }
            }

            s.sigma = generateProcessingTime();

        }

        void output(const SendingBufferState &s) const override
        {
            if(!s.buffer.empty())
            {            
                BufferPacket bufferPacket = s.buffer.front();
                out[bufferPacket.portNumber]->addMessage(bufferPacket.packet);
            }
        }

        [[nodiscard]] double timeAdvance(const SendingBufferState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
