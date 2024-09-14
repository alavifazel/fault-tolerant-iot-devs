 
#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <string>
#include <map>

namespace cadmium::iot
{
    template<class T>
    class Message {

    private:
        std::string originator;
        std::string deviceType;
        T data;
    public:
        Message(std::string originator, std::string deviceType, T data = 0):
            originator(originator),
            deviceType(deviceType),
            data(data)
        {}
        Message() {}

        // Getter / Setters
        T getData() const  { return this->data; }
        void setData(T data) { this->data = data; }
        void setOrigin(std::string originator) { this->originator = originator; } 
        std::string getOrigin() const { return this->originator; }
        std::string getDeviceType()  const { return this->deviceType; }

    };

    template<class T>
    std::ostream &operator<<(std::ostream &out, const Message<T>&s)
    {
        return out;
    }
}

#endif
