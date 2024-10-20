#include "auth_plugin.h"
#include <stdio.h>
#include <string.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include <jwt.h>

#define PUBLIC_KEY "-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA...\n"\
"-----END PUBLIC KEY-----"


mosq_plugin_EXPORT int mosquitto_plugin_version(int supported_version_count,const int *supported_versions){
    return 5;
}

/*
// Function to verify JWT token
int verify_jwt(const char *token) {
    jwt_t *jwt;

    // Parse the token
    if (jwt_decode(&jwt, token, (const unsigned char *)PUBLIC_KEY, strlen(PUBLIC_KEY)) != 0) {
        printf("Invalid token\n");
        return 0;
    }

    // Check the algorithm
    if (jwt_get_alg(jwt) != JWT_ALG_RS256) {
        printf("Invalid algorithm\n");
        jwt_free(jwt);
        return 0;
    }

    // You can add more checks here, like expiration time, issuer, etc.
    // For example:
    // time_t now = time(NULL);
    // if (jwt_get_grant_int(jwt, "exp") < now) {
    //     printf("Token expired\n");
    //     jwt_free(jwt);
    //     return 0;
    // }

    jwt_free(jwt);
    return 1;  // Token is valid
}
*/
int plugin_authenticate_qos0(const struct mosquitto_message *msg) {
    // Check if the payload starts with "Bearer "
    if (strncmp((char *)msg->payload, "Bearer ", 7) != 0) {
        printf("Invalid token format\n");
        return MOSQ_ERR_AUTH;
    }
    /*
    // Extract the token (skip "Bearer " prefix)
    const char *token = (char *)msg->payload + 7;

    // Verify the JWT token
    if (verify_jwt(token)) {
        return MOSQ_ERR_SUCCESS; // Return success if authenticated
    } else {
        return MOSQ_ERR_AUTH; // Return authentication failure
    }
    */
   return 0; //esto no va
}

// Callback for when a QoS 0 message is received
int on_message_qos0(int event, void *event_data, void *userdata) {
    struct mosquitto_evt_message *msg_evt = (struct mosquitto_evt_message *)event_data;
    struct mosquitto *context = msg_evt->client;
    uint8_t qos = msg_evt->qos;

    // Log the message details
    mosquitto_log_printf(MOSQ_LOG_INFO, "Received QOS: %u\n", qos);
    mosquitto_log_printf(MOSQ_LOG_INFO, "Message received on topic: %s", msg_evt->topic);
    mosquitto_log_printf(MOSQ_LOG_INFO, "Payload: %.*s\n", msg_evt->payloadlen, (char *)msg_evt->payload);

    // Only process QoS 0 messages
    /*
    if (qos == 0) {
        // Authenticate the message
        int rc = plugin_authenticate_qos0(msg_evt->payload);
        if (rc != MOSQ_ERR_SUCCESS) {
            printf("Authentication failed for client %s\n", mosquitto_client_id(context));
            return MOSQ_ERR_AUTH;
        }
    }
    */
    // Allow message processing to continue if authenticated
    return MOSQ_ERR_SUCCESS;
}

// Plugin initialization function
int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **userdata, struct mosquitto_opt *options, int option_count) {
    (void)options;          //recasteo la variable a void asi no salen warnings por no usar el dato
    (void)option_count; 
    
    mosquitto_log_printf(MOSQ_LOG_INFO, "Plugin initialized successfully.\n");
    mosquitto_callback_register(identifier, MOSQ_EVT_MESSAGE, on_message_qos0, NULL, userdata);
    return MOSQ_ERR_SUCCESS;
}

// Plugin cleanup function
int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count) {
    (void)user_data; 
    (void)opts;      
    (void)opt_count; 

    mosquitto_callback_unregister(NULL, MOSQ_EVT_MESSAGE, on_message_qos0, NULL);
    return MOSQ_ERR_SUCCESS;
}
