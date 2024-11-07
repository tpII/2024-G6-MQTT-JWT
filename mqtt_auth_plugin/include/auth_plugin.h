#ifndef AUTH_PLUGIN_H
#define AUTH_PLUGIN_H

#include <mosquitto.h>
#include <stdbool.h>

int plugin_authenticate_qos0(const struct mosquitto_message *msg);

int on_message_qos0(int event, void *event_data, void *userdata);

#endif

