/**
 * OQuake - OASIS STAR API Integration Implementation
 *
 * Integrates Quake with the OASIS STAR API so keys collected in ODOOM
 * can open doors in OQuake and vice versa.
 *
 * Integration Points:
 * 1. Key pickup -> add to STAR inventory (silver_key, gold_key)
 * 2. Door touch -> check local key first, then cross-game (Doom keycards)
 */

#include "quakedef.h"
#include "oquake_star_integration.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static star_api_config_t g_star_config;
static int g_star_initialized = 0;

static const char* get_key_description(const char* key_name) {
    if (strcmp(key_name, OQUAKE_ITEM_SILVER_KEY) == 0)
        return "Silver Key - Opens silver-marked doors";
    if (strcmp(key_name, OQUAKE_ITEM_GOLD_KEY) == 0)
        return "Gold Key - Opens gold-marked doors";
    return "Key from OQuake";
}

void OQuake_STAR_Init(void) {
    g_star_config.base_url = "https://star-api.oasisplatform.world/api";
    g_star_config.api_key = getenv("STAR_API_KEY");
    g_star_config.avatar_id = getenv("STAR_AVATAR_ID");
    g_star_config.timeout_seconds = 10;

    star_api_result_t result = star_api_init(&g_star_config);
    if (result != STAR_API_SUCCESS) {
        printf("OQuake STAR API: Failed to initialize: %s\n", star_api_get_last_error());
        return;
    }

    const char* username = getenv("STAR_USERNAME");
    const char* password = getenv("STAR_PASSWORD");

    if (username && password) {
        result = star_api_authenticate(username, password);
        if (result == STAR_API_SUCCESS) {
            g_star_initialized = 1;
            printf("OQuake STAR API: Authenticated via SSO. Cross-game features enabled.\n");
            return;
        }
        printf("OQuake STAR API: SSO authentication failed: %s\n", star_api_get_last_error());
    }

    if (g_star_config.api_key && g_star_config.avatar_id) {
        g_star_initialized = 1;
        printf("OQuake STAR API: Initialized with API key. Cross-game features enabled.\n");
    } else {
        printf("OQuake STAR API: Warning - No authentication configured. Set STAR_USERNAME/STAR_PASSWORD or STAR_API_KEY/STAR_AVATAR_ID.\n");
    }
}

void OQuake_STAR_Cleanup(void) {
    if (g_star_initialized) {
        star_api_cleanup();
        g_star_initialized = 0;
        printf("OQuake STAR API: Cleaned up.\n");
    }
}

void OQuake_STAR_OnKeyPickup(const char* key_name) {
    if (!g_star_initialized || !key_name)
        return;

    star_api_result_t result = star_api_add_item(
        key_name,
        get_key_description(key_name),
        "OQuake",
        "KeyItem"
    );

    if (result == STAR_API_SUCCESS)
        printf("OQuake STAR API: Added %s to cross-game inventory.\n", key_name);
    else
        printf("OQuake STAR API: Failed to add %s: %s\n", key_name, star_api_get_last_error());
}

/**
 * Cross-game key mapping (OQuake <-> ODOOM):
 * - silver_key door: accept silver_key (Quake) or red_keycard (Doom)
 * - gold_key door:   accept gold_key (Quake) or blue_keycard (Doom) or yellow_keycard (Doom)
 */
static int check_cross_game_door(const char* required_key) {
    if (strcmp(required_key, OQUAKE_ITEM_SILVER_KEY) == 0) {
        if (star_api_has_item(OQUAKE_ITEM_SILVER_KEY) || star_api_has_item(ODOOM_ITEM_RED_KEYCARD) || star_api_has_item(ODOOM_ITEM_SKULL_KEY))
            return 1;
    } else if (strcmp(required_key, OQUAKE_ITEM_GOLD_KEY) == 0) {
        if (star_api_has_item(OQUAKE_ITEM_GOLD_KEY) || star_api_has_item(ODOOM_ITEM_BLUE_KEYCARD) || star_api_has_item(ODOOM_ITEM_YELLOW_KEYCARD))
            return 1;
    }
    return 0;
}

int OQuake_STAR_CheckDoorAccess(const char* door_name, const char* required_key) {
    if (!g_star_initialized || !required_key)
        return 0;

    if (star_api_has_item(required_key)) {
        printf("OQuake STAR API: Door '%s' opened using cross-game key: %s\n", door_name ? door_name : "", required_key);
        star_api_use_item(required_key, door_name ? door_name : "oquake_door");
        return 1;
    }

    if (check_cross_game_door(required_key)) {
        const char* used = NULL;
        if (strcmp(required_key, OQUAKE_ITEM_SILVER_KEY) == 0) {
            if (star_api_has_item(ODOOM_ITEM_RED_KEYCARD)) used = ODOOM_ITEM_RED_KEYCARD;
            else if (star_api_has_item(ODOOM_ITEM_SKULL_KEY)) used = ODOOM_ITEM_SKULL_KEY;
        } else if (strcmp(required_key, OQUAKE_ITEM_GOLD_KEY) == 0) {
            if (star_api_has_item(ODOOM_ITEM_BLUE_KEYCARD)) used = ODOOM_ITEM_BLUE_KEYCARD;
            else if (star_api_has_item(ODOOM_ITEM_YELLOW_KEYCARD)) used = ODOOM_ITEM_YELLOW_KEYCARD;
        }
        if (used) {
            printf("OQuake STAR API: Door '%s' opened using ODOOM key: %s\n", door_name ? door_name : "", used);
            star_api_use_item(used, door_name ? door_name : "oquake_door");
            return 1;
        }
    }

    return 0;
}

void OQuake_STAR_OnItemPickup(const char* item_name, const char* item_description) {
    if (!g_star_initialized || !item_name)
        return;

    star_api_result_t result = star_api_add_item(
        item_name,
        item_description ? item_description : "Item from OQuake",
        "OQuake",
        "Miscellaneous"
    );

    if (result == STAR_API_SUCCESS)
        printf("OQuake STAR API: Added %s to cross-game inventory.\n", item_name);
}

int OQuake_STAR_HasCrossGameKeycard(const char* keycard_name) {
    if (!g_star_initialized || !keycard_name)
        return 0;
    return star_api_has_item(keycard_name) ? 1 : 0;
}
