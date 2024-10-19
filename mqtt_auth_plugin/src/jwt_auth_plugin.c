#include "auth_plugin.h"
#include <stdio.h>
#include <string.h>
#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include <jwt.h>

#define PUBLIC_KEY "-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA...\n"\
"-----END PUBLIC KEY-----"

// Function to verify JWT token
int verify_jwt(const char *token) {
    jwt_t *jwt;
    int ret = 0;

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

int plugin_authenticate_qos0(const struct mosquitto_message *msg) {
    // Check if the payload starts with "Bearer "
    if (strncmp((char *)msg->payload, "Bearer ", 7) != 0) {
        printf("Invalid token format\n");
        return MOSQ_ERR_AUTH;
    }

    // Extract the token (skip "Bearer " prefix)
    const char *token = (char *)msg->payload + 7;

    // Verify the JWT token
    if (verify_jwt(token)) {
        return MOSQ_ERR_SUCCESS; // Return success if authenticated
    } else {
        return MOSQ_ERR_AUTH; // Return authentication failure
    }
}

// Callback for when a QoS 0 message is received
int on_message_qos0(struct mosquitto *context, void *userdata, struct mosquitto_message *msg) {
    // Only process QoS 0 messages
    if (msg->qos == 0) {
        int rc = plugin_authenticate_qos0(msg);
        if (rc != MOSQ_ERR_SUCCESS) {
            // Use the accessor function instead of accessing context->id directly
            printf("Authentication failed for client %s\n", mosquitto_client_id(context));
            return MOSQ_ERR_AUTH;
        }
    }

    // Allow message processing to continue if authenticated
    return MOSQ_ERR_SUCCESS;
}


// Plugin initialization function
int mosquitto_plugin_init(void *user_data, struct mosquitto_opt *opts, int opt_count) {
    // Register the QoS 0 message callback
    mosquitto_callback_register(NULL, MOSQ_EVT_MESSAGE, on_message_qos0, NULL, NULL);

    printf("QoS 0 Authentication Plugin Initialized\n");
    return MOSQ_ERR_SUCCESS;
}

// Plugin cleanup function
int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count) {
    // Unregister the callback
    mosquitto_callback_unregister(NULL, MOSQ_EVT_MESSAGE, on_message_qos0, NULL);
    printf("QoS 0 Authentication Plugin Cleaned up\n");
    return MOSQ_ERR_SUCCESS;
}
