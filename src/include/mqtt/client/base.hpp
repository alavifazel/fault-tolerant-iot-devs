#ifndef MQTT_CLIENT_BASE_HPP_
#define MQTT_CLIENT_BASE_HPP_

#include <cadmium/core/modeling/atomic.hpp>
#include <iostream>
#include <queue>
#include <random>
#include "../mqtt_message.hpp"
#include "../mqtt_packet.hpp"
#include "../mqtt_client_command.hpp"
#include "utility.hpp"

namespace cadmium::iot::mqtt_client
{
    enum BasePhase
    {
        IDLE,
        DATA_AVAILABLE_FOR_CLIENT,
        DATA_AVAILABLE_TO_PUBLISH,
        DATA_AVAILABLE_TO_SUBSCRIBE,
        CONNECTED
    };

    struct BaseState
    {
        double sigma;
        BasePhase phase;
        MQTTPacket availablePacket;
        MQTTMessage newData;
        int packetIDCounter;
        BaseState() : sigma(std::numeric_limits<double>::infinity()), phase(IDLE), packetIDCounter(0) {}
    };

    std::ostream &operator<<(std::ostream &out, const BaseState &s)
    {
        out << s.sigma;
        return out;
    }

    class Base : public Atomic<BaseState>
    {
    private:
        int keepAlive;
        // When a client establishes a connection with an MQTT broker, it negotiates a Keep Alive value, which is a time interval expressed in seconds. The client must send a PINGREQ packet to the broker at least once within this interval to indicate its presence and keep the connection alive.

    public:
        Port<MQTTMessage> outData;
        Port<MQTTMessage> inData;
        Port<MQTTClientCommand> inCommand;
        Port<MQTTPacket> outBroker;
        Port<MQTTPacket> inBroker;

        double checkPeriod = 0.5;

        Base(const std::string &id, int keepAlive = 120)
            : Atomic<BaseState>(id, BaseState()),
              keepAlive(keepAlive)
        {
            // Inputs
            inData = addInPort<MQTTMessage>("in");
            outData = addOutPort<MQTTMessage>("out");
            inCommand = addInPort<MQTTClientCommand>("in_command");

            // Broker
            outBroker = addOutPort<MQTTPacket>("out_broker");
            inBroker = addInPort<MQTTPacket>("in_broker");

        }

        void internalTransition(BaseState &s) const override
        {
            s.sigma = std::numeric_limits<double>::max();
            s.phase = IDLE;

        }

        void externalTransition(BaseState &s, double e) const override
        {

            if(!inData->empty()) {
                s.newData = inData->getBag().back();
                s.packetIDCounter++;
                if(s.newData.getMQTTMessageType() == MQTTMessageType::PUBLISH)
                {
                    log("<CLIENT_BASE>: Data available to publish.");
                    s.phase = DATA_AVAILABLE_TO_PUBLISH;
                    s.sigma = 0;
                } else if(s.newData.getMQTTMessageType() == MQTTMessageType::SUBSCRIBE)
                {
                    log("<CLIENT_BASE>: Data available for subscribe.");
                    s.phase = DATA_AVAILABLE_TO_SUBSCRIBE;
                    s.sigma = 0;
                }

            }

            if (!inBroker->empty())
            {
                // log("<CLIENT_BASE> Received new packet from buffer.");
                auto packet = inBroker->getBag().back();
                auto msgType = packet.getMessageType();
                auto msgBody = packet.getBody();
                switch (msgType)
                {
                case MQTTPacketType::CONNACK:
                {
                    if (msgBody["returnCode"] == "0")
                    {
                        log("Broker return successful return code.");
                        s.phase = CONNECTED;
                        s.sigma = this->keepAlive;
                    }
                    else
                    { // TODO: if it was "1" log this, "2" log that.
                        log("Broker return error return code.");
                        s.phase = IDLE;
                        s.sigma = std::numeric_limits<double>::max();
                    }
                    break;
                }
                case MQTTPacketType::SUBACK:
                {
                    log("<CLIENT_BASE> SUBACK packet recieved.");
                    break;
                }                
                default:
                {
                    log("<CLINET_BASE>" + id  +" Packet available for client.");
                    s.phase = DATA_AVAILABLE_FOR_CLIENT;
                    s.availablePacket = packet;
                    s.sigma = 0;
                    break;
                }
                }
            }
        }

        void output(const BaseState &s) const override
        {
            switch(s.phase) {
                case DATA_AVAILABLE_FOR_CLIENT:
                {
                    outData->addMessage(
                            MQTTMessage(
                            "broker",
                            s.availablePacket.getBody()["payload"],
                            s.availablePacket.getBody()["topic"],
                            MQTTMessageType::PUBLISH,
                            0
                            )
                    );
                    break;
                }
                case DATA_AVAILABLE_TO_PUBLISH:
                {
                    log("<CLIENT_BASE>: Sending new data to the ack buffer.");
                    outBroker->addMessage(
                        MQTTPacket::createPUBLISH(
                            s.packetIDCounter,
                            s.newData.getTopic(),
                            false,
                            s.newData.getQoS(),
                            false,
                            s.newData.getData(), true)
                    );
                    break;
                }
                
                case DATA_AVAILABLE_TO_SUBSCRIBE:
                {
                    outBroker->addMessage(
                        MQTTPacket::createSUBSCRIBE(
                            s.packetIDCounter,
                            s.newData.getTopic(),
                            s.newData.getQoS()
                        )
                    );
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