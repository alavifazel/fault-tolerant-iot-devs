#ifndef MQTT_CLIENT_COMMAND_HPP_
#define MQTT_CLIENT_COMMAND_HPP_

#include <string>
#include <map>

namespace cadmium::iot
{
    using std::map;
    using std::string;

    enum class MQTTClientCommandType
    {
        CONNECT,
        DISCONNECT
    };

    class MQTTClientCommand { 

    private:
        MQTTClientCommandType commandType;

    public:
        MQTTClientCommand(MQTTClientCommandType commandType): commandType(commandType) {}
        MQTTClientCommandType getCommand() const { return this->commandType; }
    };

    std::ostream &operator<<(std::ostream &out, const MQTTClientCommand &s)
    {
        return out;
    }
}

#endif