/**
 * Quake - OASIS STAR API Integration Implementation
 * Note: This is a native C bridge for QuakeC integration
 */

#include "quake_star_integration.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static star_api_config_t g_star_config;
static int g_star_initialized = 0;

void Quake_STAR_Init(void) {
    g_star_config.base_url = "https://star-api.oasisplatform.world/api";
    g_star_config.api_key = getenv("STAR_API_KEY");
    g_star_config.avatar_id = getenv("STAR_AVATAR_ID");
    g_star_config.timeout_seconds = 10;
    
    star_api_result_t result = star_api_init(&g_star_config);
    if (result != STAR_API_SUCCESS) {
        printf("STAR API: Failed to initialize: %s\n", star_api_get_last_error());
        return;
    }
    
    // Try SSO authentication first
    const char* username = getenv("STAR_USERNAME");
    const char* password = getenv("STAR_PASSWORD");
    
    if (username && password) {
        result = star_api_authenticate(username, password);
        if (result == STAR_API_SUCCESS) {
            g_star_initialized = 1;
            printf("STAR API: Authenticated via SSO. Cross-game features enabled.\n");
            return;
        } else {
            printf("STAR API: SSO authentication failed: %s\n", star_api_get_last_error());
        }
    }
    
    // Fall back to API key
    if (g_star_config.api_key && g_star_config.avatar_id) {
        g_star_initialized = 1;
        printf("STAR API: Initialized with API key. Cross-game features enabled.\n");
    } else {
        printf("STAR API: Warning - No authentication configured.\n");
    }
}

void Quake_STAR_Cleanup(void) {
    if (g_star_initialized) {
        star_api_cleanup();
        g_star_initialized = 0;
        printf("STAR API: Cleaned up.\n");
    }
}

void Quake_STAR_OnKeyPickup(const char* key_name) {
    if (!g_star_initialized || !key_name) {
        return;
    }
    
    const char* description = NULL;
    if (strcmp(key_name, QUAKE_ITEM_SILVER_KEY) == 0) {
        description = "Silver Key - Opens silver-marked doors";
    } else if (strcmp(key_name, QUAKE_ITEM_GOLD_KEY) == 0) {
        description = "Gold Key - Opens gold-marked doors";
    } else {
        description = "Key from Quake";
    }
    
    star_api_result_t result = star_api_add_item(
        key_name,
        description,
        "Quake",
        "KeyItem"
    );
    
    if (result == STAR_API_SUCCESS) {
        printf("STAR API: Added %s to cross-game inventory.\n", key_name);
    } else {
        printf("STAR API: Failed to add %s: %s\n", key_name, star_api_get_last_error());
    }
}

int Quake_STAR_CheckDoorAccess(const char* door_name, const char* required_key) {
    if (!g_star_initialized || !required_key) {
        return 0;
    }
    
    // Check cross-game inventory
    if (star_api_has_item(required_key)) {
        printf("STAR API: Door '%s' opened using cross-game key: %s\n", door_name, required_key);
        star_api_use_item(required_key, door_name);
        return 1;
    }
    
    // Also check for Doom keycard equivalents
    if (strcmp(required_key, "silver_key") == 0) {
        if (star_api_has_item("red_keycard")) {
            printf("STAR API: Using Doom red keycard to open Quake door!\n");
            star_api_use_item("red_keycard", door_name);
            return 1;
        }
    } else if (strcmp(required_key, "gold_key") == 0) {
        if (star_api_has_item("blue_keycard")) {
            printf("STAR API: Using Doom blue keycard to open Quake door!\n");
            star_api_use_item("blue_keycard", door_name);
            return 1;
        }
    }
    
    return 0;
}

void Quake_STAR_OnItemPickup(const char* item_name, const char* item_description) {
    if (!g_star_initialized || !item_name) {
        return;
    }
    
    star_api_result_t result = star_api_add_item(
        item_name,
        item_description ? item_description : "Item from Quake",
        "Quake",
        "Miscellaneous"
    );
    
    if (result == STAR_API_SUCCESS) {
        printf("STAR API: Added %s to cross-game inventory.\n", item_name);
    }
}

int Quake_STAR_HasCrossGameKeycard(const char* keycard_name) {
    if (!g_star_initialized || !keycard_name) {
        return 0;
    }
    
    return star_api_has_item(keycard_name) ? 1 : 0;
}



