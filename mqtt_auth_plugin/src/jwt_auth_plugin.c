#include "auth_plugin.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include <jwt.h>

#define SECRET_KEY "taller-proyecto-2"
#define MAX_TOKEN_SIZE 1024
#define MAX_PAYLOAD_SIZE 512

// Helper function to verify JWT token
int verify_jwt(const char *token, char *payload) {
    jwt_t *jwt = NULL;
    int result = -1;
    
    if (!token) {
        mosquitto_log_printf(MOSQ_LOG_ERR, "No token provided");
        return -1;
    }

    int decode_res = jwt_decode(&jwt, token, (const unsigned char *)SECRET_KEY, strlen(SECRET_KEY));
    if (decode_res != 0) {
        mosquitto_log_printf(MOSQ_LOG_ERR, "JWT decode failed: %s",token);
        return -1;
    }
    
    const char *payload_str = jwt_get_grants_json(jwt, NULL);
    if (payload_str) {
        size_t grants_len = strlen(payload_str);
        if (grants_len >= MAX_PAYLOAD_SIZE) {
            mosquitto_log_printf(MOSQ_LOG_ERR, "JWT grants too large");
            jwt_free(jwt);
            return -1;
        }
        memcpy(payload, payload_str, grants_len);
        payload[grants_len] = '\0';  // Ensure null termination
        mosquitto_log_printf(MOSQ_LOG_INFO, "JWT grants: %s", payload);
    }

    // Verify token hasn't expired (if exp claim exists)
    time_t now = time(NULL);
    long exp = 0;
    exp = jwt_get_grant_int(jwt, "exp");
    if (exp > 0 && now > exp) {
        mosquitto_log_printf(MOSQ_LOG_WARNING, "JWT token has expired");
        result = -1;
    } else {
        result = 0; // Token is valid
    }

    jwt_free(jwt);
    return result;
}

// Callback for when a message is received
int on_message_qos0(int event, void *event_data, void *userdata) {
    struct mosquitto_evt_message *msg_evt = (struct mosquitto_evt_message *)event_data;
    struct mosquitto *context = msg_evt->client;
    int rc = MOSQ_ERR_SUCCESS;
    char token_buf[MAX_TOKEN_SIZE];
    char *new_payload = NULL;
    size_t payload_len;

    // Basic validation
    if (!msg_evt || !msg_evt->payload || msg_evt->payloadlen <= 0) {
        mosquitto_log_printf(MOSQ_LOG_ERR, "Invalid message received");
        return MOSQ_ERR_INVAL;
    }

    // Ensure the payload isn't too large and is null-terminated
    if (msg_evt->payloadlen >= MAX_TOKEN_SIZE) {
        mosquitto_log_printf(MOSQ_LOG_ERR, "Token too large");
        return MOSQ_ERR_INVAL;
    }

    // Copy payload to ensure null-termination
    memcpy(token_buf, msg_evt->payload, msg_evt->payloadlen);
    token_buf[msg_evt->payloadlen] = '\0';

    // Allocate space for the payload
    new_payload = malloc(MAX_PAYLOAD_SIZE);
    if (!new_payload) {
        mosquitto_log_printf(MOSQ_LOG_ERR, "Memory allocation failed");
        return MOSQ_ERR_NOMEM;
    }
    memset(new_payload, 0, MAX_PAYLOAD_SIZE);

    // Log attempt
    mosquitto_log_printf(MOSQ_LOG_INFO, 
        "Processing auth request from client %s", 
        mosquitto_client_id(context));

    // Verify the JWT token
    rc = verify_jwt(token_buf, new_payload);
    if (rc != 0) {
        mosquitto_log_printf(MOSQ_LOG_WARNING, 
            "Authentication failed for client %s", 
            mosquitto_client_id(context));
        return MOSQ_ERR_AUTH;
    }

    mosquitto_log_printf(MOSQ_LOG_INFO, 
        "Authentication successful for client %s", 
        mosquitto_client_id(context));
    
    // Get the actual length of the payload
    payload_len = strlen(new_payload);
    
    // Update the message event with the new payload
    msg_evt->payload = new_payload;
    msg_evt->payloadlen = payload_len;

    mosquitto_log_printf(MOSQ_LOG_INFO, "Payload: %s", new_payload);
    mosquitto_log_printf(MOSQ_LOG_INFO, "Payload Length: %zu", payload_len);

    return MOSQ_ERR_SUCCESS;
}

// Plugin version (keep as is)
mosq_plugin_EXPORT int mosquitto_plugin_version(int supported_version_count, const int *supported_versions) {
    return 5;
}

// Plugin initialization (keep as is)
int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **userdata, struct mosquitto_opt *options, int option_count) {
    (void)options;
    (void)option_count;
    
    mosquitto_log_printf(MOSQ_LOG_INFO, "JWT Auth Plugin initialized!");
    mosquitto_callback_register(identifier, MOSQ_EVT_MESSAGE, on_message_qos0, NULL, userdata);
    return MOSQ_ERR_SUCCESS;
}

// Plugin cleanup (keep as is)
int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count) {
    (void)user_data;
    (void)opts;
    (void)opt_count;

    mosquitto_callback_unregister(NULL, MOSQ_EVT_MESSAGE, on_message_qos0, NULL);
    return MOSQ_ERR_SUCCESS;
}