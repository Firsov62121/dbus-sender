#include <iostream>

#include <dbus/dbus.h>

#include "sender.h"

namespace sender {

static std::string call_method(std::initializer_list<const char *> str_args, const char *method) {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusConnection* conn;
    DBusError err;
    DBusPendingCall* pending;
    int ret;

    dbus_error_init(&err);

    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err))
        dbus_error_free(&err);
    if (conn == nullptr)
        throw SenderError("Could not create connection!");

    ret = dbus_bus_request_name(conn, "org.firsov.caller", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
    if (dbus_error_is_set(&err))
        dbus_error_free(&err);
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
        throw SenderError("Requested name has primary owner!");

    msg = dbus_message_new_method_call("org.firsov.server",
                                       "/test/method/Object",
                                       "org.firsov.register",
                                       method);
    if (msg == nullptr)
        throw SenderError("Message Null!");

    dbus_message_iter_init_append(msg, &args);
    for (const char * arg : str_args) {
        if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &arg))
            throw SenderError("Out Of Memory!");
    }

    // timeout: 1 seconds
    if (!dbus_connection_send_with_reply (conn, msg, &pending, 1e4))
        throw SenderError("Out Of Memory!");
    if (pending == nullptr)
        throw SenderError("Pending Call Null!");
    dbus_connection_flush(conn);

    dbus_message_unref(msg);
    dbus_pending_call_block(pending);

    msg = dbus_pending_call_steal_reply(pending);
    if (msg == nullptr)
        throw SenderError("Reply Null!");
    dbus_pending_call_unref(pending);


    const char *result_c;
    if (!dbus_message_iter_init(msg, &args))
        throw SenderError("Message has no arguments!");
    else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        throw SenderError("Argument is not string!");
    else
        dbus_message_iter_get_basic(&args, &result_c);
    
    std::string res(result_c);
    dbus_message_unref(msg);   
    dbus_connection_unref(conn);
    return res;
}

std::string add_file(const std::string &filename, const std::string &how_to_run) {
    return call_method({ filename.c_str(), how_to_run.c_str() }, "Register");
}

void open_file(const std::string &filename) {
    std::string program = call_method({ filename.c_str() }, "GetProgram");
    if (program.empty())
        throw SenderError("No command was specified to open this file!");
    std::string run_command = program + " \'" + filename + "\'";
    try {
        system(run_command.c_str());
    } catch(const std::exception& ex) {
        std::cerr << "couldn't run command: " << run_command << ", problem: " << ex.what() << "\n";
    }
}

} // namespace sender
