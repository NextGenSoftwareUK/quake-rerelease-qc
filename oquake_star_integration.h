/**
 * OQuake - OASIS STAR API Integration
 *
 * This header provides integration hooks for OQuake (Quake + STAR API)
 * to connect with the OASIS STAR API for cross-game item sharing with ODOOM.
 *
 * Integration Points:
 * - Key collection (key_touch / item_key1 / item_key2)
 * - Door opening (door_touch)
 * - Item pickup tracking
 *
 * Cross-game keys: Keys collected in ODOOM (red/blue/yellow/skull) can open
 * OQuake doors (silver/gold) and vice versa.
 */

#ifndef OQUAKE_STAR_INTEGRATION_H
#define OQUAKE_STAR_INTEGRATION_H

/* Use "star_api.h" with include path set to NativeWrapper or Quake tree */
#include "star_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OQuake key item names (same as STAR API canonical names) */
#define OQUAKE_ITEM_SILVER_KEY   "silver_key"
#define OQUAKE_ITEM_GOLD_KEY     "gold_key"
/* Doom keycards (for cross-game door check) */
#define ODOOM_ITEM_RED_KEYCARD  "red_keycard"
#define ODOOM_ITEM_BLUE_KEYCARD "blue_keycard"
#define ODOOM_ITEM_YELLOW_KEYCARD "yellow_keycard"
#define ODOOM_ITEM_SKULL_KEY    "skull_key"

/* Initialize STAR API integration for OQuake */
void OQuake_STAR_Init(void);

/* Cleanup STAR API integration */
void OQuake_STAR_Cleanup(void);

/* Called when player picks up a key (silver_key or gold_key) */
void OQuake_STAR_OnKeyPickup(const char* key_name);

/* Called when player touches a key door. Returns 1 if door can open (local or cross-game key), 0 otherwise. */
int OQuake_STAR_CheckDoorAccess(const char* door_name, const char* required_key);

/* Called when player picks up any item */
void OQuake_STAR_OnItemPickup(const char* item_name, const char* item_description);

/* Check if player has a key from another game (e.g. ODOOM) */
int OQuake_STAR_HasCrossGameKeycard(const char* keycard_name);

#ifdef __cplusplus
}
#endif

#endif /* OQUAKE_STAR_INTEGRATION_H */
