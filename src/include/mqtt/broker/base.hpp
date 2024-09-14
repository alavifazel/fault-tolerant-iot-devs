#ifndef MQTT_BROKER_BASE_HPP_
#define MQTT_BROKER_BASE_HPP_

#ifndef SIMULATION
#include "cadmium/modeling/devs/atomic.hpp"
#else
#include <cadmium/core/modeling/atomic.hpp>
#endif

#include <cadmium/core/modeling/atomic.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <set>
#include <map>
#include "../mqtt_packet.hpp"
#include "../../utility.hpp"
#include "../broker_internal_message.hpp"

namespace cadmium::iot::mqtt_broker
{
    using std::vector;

    enum class BasePhase
    {
        PASSIVE,
        PACKET_ARRIVED,
        PACKET_ARRIVED_FOR_SUBSCRIPTION        
    };

    struct BaseState
    {
        double sigma;
        BrokerInternalMessage newBrokerInternalMessage;
        vector<BrokerInternalMessage> arrivedAckPackets;
        // map<int, BrokerInternalMessage> newBrokerInternalMessage;
        int packetIDCounter; // To Publish to the subscribed clients
        BasePhase phase;

        // Maps every topic to the lits of client (their port numbers) that are subscribed to that topic
        map<string, vector<int>> subscriptions;
        BaseState()
            : sigma(std::numeric_limits<double>::infinity()),
              packetIDCounter(0),
              phase(BasePhase::PASSIVE)
        {

        }
    };

    std::ostream &operator<<(std::ostream &out, const BaseState &s)
    {
        out << s.sigma;
        return out;
    }

    class Base : public Atomic<BaseState>
    {
    private:

    public:
        Port<BrokerInternalMessage> in;
        Port<vector<BrokerInternalMessage>> out;   

        Base(const std::string &id)
            : Atomic<BaseState>(id, BaseState())
        {
            in = addInPort<BrokerInternalMessage>("in");
            out = addOutPort<vector<BrokerInternalMessage>>("out");
        }

        void internalTransition(BaseState &s) const override
        {
            switch(s.phase) {
                case BasePhase::PACKET_ARRIVED:
                case BasePhase::PACKET_ARRIVED_FOR_SUBSCRIPTION:
                {
                    s.phase = BasePhase::PASSIVE;
                    break;
                }
            }
            s.sigma = std::numeric_limits<double>::infinity();
        }

        void externalTransition(BaseState &s, double e) const override
        {
            if (!in->empty())
            {
                auto internalPacket = in->getBag().back();
                s.newBrokerInternalMessage = internalPacket;
                auto packet = internalPacket.packet;

                if(internalPacket.packet.getMessageType() == MQTTPacketType::SUBSCRIBE)
                {
                    s.subscriptions[packet.getBody()["topic"]].push_back(internalPacket.portNumber);
                    s.phase = BasePhase::PACKET_ARRIVED_FOR_SUBSCRIPTION;
                }
                else
                {

                    if(packet.getMessageType() == MQTTPacketType::PUBLISH) {
                        s.packetIDCounter += s.subscriptions[packet.getBody()["topic"]].size();
                    }                 
                    s.phase = BasePhase::PACKET_ARRIVED;
                }
                s.sigma = 0;
            }          
        }

        void output(const BaseState &s) const override
        {
            switch(s.phase) {
                case BasePhase::PACKET_ARRIVED:
                {
                    vector<BrokerInternalMessage> output;
                    // output.push_back(BrokerInternalMessage(s.newBrokerInternalMessage.portNumber, s.newBrokerInternalMessage.packet));
                    if(s.newBrokerInternalMessage.packet.getMessageType() == MQTTPacketType::PUBLISH)
                    {
                        auto listOfSubscribedClients = s.subscriptions.at(s.newBrokerInternalMessage.packet.getBody()["topic"]);
                        for(int i = 0; i < listOfSubscribedClients.size(); ++i)
                        {
                            log("<BROKER_BASE> Broadcasting to clinets on port: " + std::to_string(i));
                            output.push_back(BrokerInternalMessage(
                                listOfSubscribedClients[i], 
                                MQTTPacket::createPUBLISH(
                                    s.packetIDCounter + i, 
                                    s.newBrokerInternalMessage.packet.getBody()["topic"],
                                    false, 
                                    0,
                                    true, 
                                    s.newBrokerInternalMessage.packet.getBody()["payload"],
                                    true
                                    )
                            ));
                        }
                    }
                    out->addMessage(output);

                    break;
                }
                case BasePhase::PACKET_ARRIVED_FOR_SUBSCRIPTION:
                {
                    vector<BrokerInternalMessage> output;
                    log("<BROKER_BASE> sending SUBACK for packet ID:"
                            + s.newBrokerInternalMessage.packet.getBody()["packetID"]
                            + "through port" + to_string(s.newBrokerInternalMessage.portNumber));
                    output.push_back(
                        BrokerInternalMessage(s.newBrokerInternalMessage.portNumber, MQTTPacket::createSUBPACK(
                                std::stoi(s.newBrokerInternalMessage.packet.getBody()["packetID"]),
                                std::stoi(s.newBrokerInternalMessage.packet.getBody()["QoSLevel"])
                        ))  
                    );

                    out->addMessage( // send the SUBPACK back to the Subscriber
                            output);

                    break;
                }
            }


        }

        [[nodiscard]] double timeAdvance(const BaseState &s) const override
        {
            return s.sigma;
        }
    };
}


#endif