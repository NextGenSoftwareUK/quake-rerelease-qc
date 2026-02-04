/**
 * Quake - OASIS STAR API Integration
 * Note: Quake uses QuakeC (interpreted), so we need a native C bridge
 */

#ifndef QUAKE_STAR_INTEGRATION_H
#define QUAKE_STAR_INTEGRATION_H

#include "star_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// Quake-specific item types
#define QUAKE_ITEM_SILVER_KEY    "silver_key"
#define QUAKE_ITEM_GOLD_KEY     "gold_key"
#define QUAKE_ITEM_RUNE1        "rune_1"
#define QUAKE_ITEM_RUNE2        "rune_2"
#define QUAKE_ITEM_QUAD         "quad_damage"
#define QUAKE_ITEM_PENTAGRAM    "pentagram"

// Initialize STAR API integration for Quake
void Quake_STAR_Init(void);

// Cleanup STAR API integration
void Quake_STAR_Cleanup(void);

// Called when player picks up a key
void Quake_STAR_OnKeyPickup(const char* key_name);

// Called when player tries to open a door
int Quake_STAR_CheckDoorAccess(const char* door_name, const char* required_key);

// Called when player picks up any item
void Quake_STAR_OnItemPickup(const char* item_name, const char* item_description);

// Check if player has a keycard from another game (e.g., Doom)
int Quake_STAR_HasCrossGameKeycard(const char* keycard_name);

#ifdef __cplusplus
}
#endif

#endif // QUAKE_STAR_INTEGRATION_H



