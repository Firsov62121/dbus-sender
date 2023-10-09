#pragma once

#include <string>
#include <unordered_map>

#include <dbus/dbus.h>

namespace dbuser {

constexpr auto iface = "org.firsov.register";

struct RegistrationError : public std::runtime_error {
    RegistrationError(std::string const &msg) : std::runtime_error(msg) {}
};

class Register {
private:
    std::string service_name;
    // filename -> program to run
    std::unordered_map<std::string, std::string> methods;
    DBusError error;
    DBusConnection *conn = nullptr;

    void  addMethod(const std::string &filename, const std::string &program);
    const char *getMethod(const std::string &filename);

    void registerAndReply(DBusMessage *msg);
    void replyMethodName(DBusMessage *msg);
    void reply(const char *reply_str, DBusMessage *msg);
    char *parseFirstArgument(DBusMessageIter &args, DBusMessage *msg);

public:
    Register(const std::string &service_name);
    ~Register();
    
    void startLoop();
};

} // namespace dbuser
