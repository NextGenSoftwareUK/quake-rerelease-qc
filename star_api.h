/**
 * OASIS STAR API - C/C++ Wrapper for Game Integration
 * Simplified version for Quake integration
 */

#ifndef STAR_API_H
#define STAR_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Configuration structure
typedef struct {
    const char* base_url;
    const char* api_key;
    const char* avatar_id;
    int timeout_seconds;
} star_api_config_t;

// Result codes
typedef enum {
    STAR_API_SUCCESS = 0,
    STAR_API_ERROR_INIT_FAILED = -1,
    STAR_API_ERROR_NOT_INITIALIZED = -2,
    STAR_API_ERROR_NETWORK = -3,
    STAR_API_ERROR_INVALID_PARAM = -4,
    STAR_API_ERROR_API_ERROR = -5
} star_api_result_t;

// Initialize STAR API
star_api_result_t star_api_init(const star_api_config_t* config);

// Authenticate using SSO
star_api_result_t star_api_authenticate(const char* username, const char* password);

// Cleanup
void star_api_cleanup(void);

// Check if player has item
bool star_api_has_item(const char* item_name);

// Add item to inventory
star_api_result_t star_api_add_item(
    const char* item_name,
    const char* description,
    const char* game_source,
    const char* item_type
);

// Use item
bool star_api_use_item(const char* item_name, const char* context);

// Get last error
const char* star_api_get_last_error(void);

#ifdef __cplusplus
}
#endif

#endif // STAR_API_H



