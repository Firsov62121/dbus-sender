#include <chrono>
#include <thread>
#include <iostream>

#include "registrator.h"

namespace dbuser {

Register::Register(const std::string &service_name)
     : service_name(service_name) {
    dbus_error_init(&error);
    
    if (nullptr == (conn = dbus_bus_get(DBUS_BUS_SESSION, &error)))
        throw RegistrationError(error.message);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set(&error))
        dbus_error_free(&error);

    int ret = dbus_bus_request_name(conn, service_name.c_str(), DBUS_NAME_FLAG_REPLACE_EXISTING , &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Error with requset name: " << error.message << "\n";
        dbus_error_free(&error);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        throw RegistrationError("Not Primary Owner!");
    }

}

Register::~Register() {
    if (conn != nullptr)
        dbus_connection_unref(conn);
}

void Register::startLoop() {

    DBusMessage *msg;
    while (true) {
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);
        if (msg == nullptr) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        try {
            if (dbus_message_is_method_call(msg, iface, "Register"))
                registerAndReply(msg);
            else if (dbus_message_is_method_call(msg, iface, "GetProgram"))
                replyMethodName(msg);
            else
                reply("", msg);
        } catch (const RegistrationError &err) {
            std::cerr << "Couldn't handle request: " << err.what() << "\n";
            dbus_connection_flush(conn);
        }
        dbus_message_unref(msg);
    }
}

void  Register::addMethod(const std::string &filename, const std::string &program) {
    methods[filename] = program;   
}

// change to copying of string if will add multithreading
const char *Register::getMethod(const std::string &filename) {
    if (methods.count(filename) != 0)
        return methods[filename].c_str();
    return "";
}

void Register::registerAndReply(DBusMessage *msg) {
    DBusMessageIter args;
    char *filename = parseFirstArgument(args, msg);
    char *program = nullptr;
    
    if (dbus_message_iter_next(&args) == FALSE)
        throw RegistrationError("No second argument was provided!");
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        throw RegistrationError("Second argument is not a string!");
    dbus_message_iter_get_basic(&args, &program);

    addMethod(filename, program);

    reply("Registered successfully!", msg);
}

void Register::replyMethodName(DBusMessage *msg) {
    DBusMessageIter args;
    char *filename = parseFirstArgument(args, msg);
    reply(getMethod(filename), msg);
}

static void memoryFinished() {
    std::cerr << "Out Of Memory!\n";
    exit(1);
}

void Register::reply(const char *reply_str, DBusMessage *msg) {
    dbus_uint32_t serial = dbus_message_get_serial(msg);
    DBusMessageIter args;
    DBusMessage *reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &reply_str))
        memoryFinished();
    if (!dbus_connection_send(conn, reply, &serial))
        memoryFinished();

    dbus_connection_flush(conn);
    dbus_message_unref(reply);
}

char *Register::parseFirstArgument(DBusMessageIter &args, DBusMessage *msg) {
    char *filename = nullptr;
    if (!dbus_message_iter_init(msg, &args))
        throw RegistrationError("Message has no arguments!");
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        throw RegistrationError("First argument is not a string!");
    dbus_message_iter_get_basic(&args, &filename);
    return filename;
}

} // namespace dbuser
