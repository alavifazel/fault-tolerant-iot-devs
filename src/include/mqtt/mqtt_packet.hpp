#ifndef MQTT_PACKET_HPP_
#define MQTT_PACKET_HPP_

#include <string>
#include <map>

namespace cadmium::iot
{
    using std::string;
    using std::to_string;
    using std::map;
    enum class MQTTPacketType
    {
        
        CONNECT,
        CONNACK,
        SUBSCRIBE,
        SUBACK,
        PUBLISH,
        PUBREL,
        PUBREC,
        PUBCOMP,
        PINGREQ, // This should be sent within the timeout period to keep the connection (if other Control packets gets sent then this is not necessary)
        DISCONNECT
    };

    class MQTTPacket
    {
    private:
        // string origin;
        map<string, string> body;
        MQTTPacketType messageType;
        bool newPublishPacket;
    public:
        MQTTPacket(MQTTPacketType messageType, map<string, string> body, bool newPublishPacket = false) :
                 messageType(messageType),
                 body(body),
                 newPublishPacket(newPublishPacket)
                                                                
        {
        }
        bool isNewPublishPacket() const { return this->newPublishPacket; };
        MQTTPacket() {}
        map<string, string> getBody() const { return this->body; }
        void setBody(map<string, string> body) { this->body = body; }
        MQTTPacketType getMessageType() const { return this->messageType; }
        
        int getPacketID() const {
            // TODO : check if it's not a CONNECT or CONNACK
            return std::stoi(this->body.at("packetID"));
        }

        int getQoSLevel() const {
            // TODO : check if it's not a CONNECT or CONNACK
            return std::stoi(this->body.at("QoSLevel"));
        }

        static MQTTPacket createCONNECT(string clientID, bool cleanSession, int keepAlive)
        {
            return MQTTPacket(MQTTPacketType::CONNECT,
                                       {{"clientID", clientID},
                                       {"cleanSession",  cleanSession ? "true" : "false" },
                                       {"keepAlive", to_string(keepAlive) }}
            );
        }

        static MQTTPacket createCONNACK(bool sessionPresent, int returnCode)
        {
            return MQTTPacket(MQTTPacketType::CONNACK,
                                     {{"sessionPresent", sessionPresent ? "true": "false"},
                                     {"returnCode", to_string(returnCode) }});
        }
        
        static MQTTPacket createPUBLISH(int packetID, 
                                        string topic,
                                        bool DUPFlag, 
                                        int QoSLevel,
                                        bool retain,
                                        string payload,
                                        bool newPublishPacket = false)
        {
            return MQTTPacket(MQTTPacketType::PUBLISH,
                                    {{"packetID", to_string(packetID)},
                                    {"topic", topic},
                                    {"DUPFlag", DUPFlag ? "true" : "false"},
                                    {"QoSLevel", to_string(QoSLevel)},
                                    {"retain", retain ? "true" : "false"},
                                    {"payload", payload}}, newPublishPacket);
        }

        static MQTTPacket createPUBREL(int packetID) {
            return MQTTPacket(MQTTPacketType::PUBREL, { {"packetID", to_string(packetID)}} );
        }

        static MQTTPacket createPUBREC(int packetID) {
            return MQTTPacket(MQTTPacketType::PUBREC, { {"packetID", to_string(packetID)}} );
        }

        static MQTTPacket createPUBCOMP(int packetID) {
            return MQTTPacket(MQTTPacketType::PUBCOMP, { {"packetID", to_string(packetID)}} );
        }
        
        static MQTTPacket createPINGREQ()
        {
            return MQTTPacket(MQTTPacketType::PINGREQ, {});
        }

        static MQTTPacket createDISCONNECT() { 
            return MQTTPacket(MQTTPacketType::DISCONNECT, {});
        }

        static std::string getMQTTPacketTypeAsString(MQTTPacket packet) {
            switch(packet.getMessageType()) {
                case MQTTPacketType::CONNECT: return "CONNECT"; break;
                case MQTTPacketType::CONNACK: return "CONNACK"; break;
                case MQTTPacketType::SUBSCRIBE: return "SUBSCRIBE"; break;
                case MQTTPacketType::SUBACK: return "SUBACK"; break;
                case MQTTPacketType::PUBLISH: return "PUBLISH"; break;
                case MQTTPacketType::PUBREL:  return "PUBREL"; break;
                case MQTTPacketType::PUBREC: return "PUBREC"; break;
                case MQTTPacketType::PUBCOMP: return "PUBCOMP"; break;
                case MQTTPacketType::PINGREQ: return "PINGREQ"; break;
                case MQTTPacketType::DISCONNECT: return "DISCONNECT"; break;
                default: return nullptr; break;
            }
        }

        static MQTTPacket createSUBSCRIBE(int packetID, string topic, int qos) { 
            return MQTTPacket(MQTTPacketType::SUBSCRIBE,
                                       {{"packetID", to_string(packetID)},
                                       {"topic",  topic },
                                       {"QoSLevel", to_string(qos) }});
        }

        static MQTTPacket createSUBPACK(int packetID, int returnCode) {
            // returnCode is either 0, 1 or 2
            return MQTTPacket(MQTTPacketType::SUBACK,
                                    {{"packetID", to_string(packetID)},
                                     {"QoSLevel", to_string(returnCode)}});
            
        }

    };
    std::ostream &operator<<(std::ostream &out, const MQTTPacket &s)
    {
        auto packetID = s.getBody()["packetID"];
        if(!packetID.empty())
        {
            out << " "<< MQTTPacket::getMQTTPacketTypeAsString(s) << " id:" << packetID ;
        } else 
        {
            out << " "<< MQTTPacket::getMQTTPacketTypeAsString(s);
        }
        return out;
    }
}
#endif