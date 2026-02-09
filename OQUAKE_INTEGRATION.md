# OQuake – STAR API integration (cross-game keys with ODOOM)

This tree contains **QuakeC** game code. The STAR API is implemented in C in this repo as **OQuake** integration; the **engine** that runs this QuakeC must call into it.

## What OQuake does

- **Key pickups** in OQuake (silver/gold) are sent to the OASIS STAR API.
- **Key doors** in OQuake can be opened by keys from **ODOOM** (red/blue/yellow/skull keycards) and vice versa.

So keys collected in Doom can open doors in Quake and vice versa.

## Files in this repo

| File | Purpose |
|------|--------|
| `oquake_star_integration.c` | OQuake STAR API integration (init, key pickup, door check) |
| `oquake_star_integration.h` | Declarations for engine |
| `oquake_version.h` | OQuake name/version for window title |
| `star_api.h` | STAR API C interface (copy from OASIS NativeWrapper if needed) |

The engine build must also link **star_api.lib** and have **star_api.dll** next to the exe (see OASIS OQuake guide).

## QuakeC integration (done)

The QuakeC in this repo **already calls** the OQuake builtins:

- **defs.qc** – Declares `OQuake_OnKeyPickup` and `OQuake_CheckDoorAccess` as extension builtins (`#0:ex_*`).
- **items.qc** – In `key_touch`, after giving the player the key, calls `OQuake_OnKeyPickup("silver_key")` or `OQuake_OnKeyPickup("gold_key")`.
- **doors.qc** – In `door_touch`, when the player doesn’t have the local key, calls `OQuake_CheckDoorAccess(self.owner.targetname, "silver_key")` or `"gold_key"`; if it returns 1, the door opens (cross-game key).

So no further QuakeC changes are needed. You only need the **engine** to implement the two builtins.

## Where the engine must hook

The **engine** must:

1. **Startup / shutdown**  
   Call `OQuake_STAR_Init()` at init and `OQuake_STAR_Cleanup()` at shutdown.

2. **Implement extension builtins**  
   Register the two extension builtins so they call into the C layer:
   - `ex_OQuake_OnKeyPickup` → call `OQuake_STAR_OnKeyPickup(keyname)` (one string arg).
   - `ex_OQuake_CheckDoorAccess` → call `OQuake_STAR_CheckDoorAccess(doorname, requiredkey)` (two string args), push float return (1 or 0).

See **`engine_oquake_hooks.c.example`** in this directory for the C wrappers and registration notes.

## QuakeC reference

- **Keys:** `quakec/items.qc` – `item_key1` (IT_KEY1, silver), `item_key2` (IT_KEY2, gold). Touch handler is `key_touch`; it calls `OQuake_OnKeyPickup` after giving the key.
- **Doors:** `quakec/doors.qc` – `door_touch`; when the player lacks the key it calls `OQuake_CheckDoorAccess`; if 1, the door opens. `self.owner.items == IT_KEY1` for silver, `IT_KEY2` for gold.

## Full integration guide

See **OASIS Omniverse OQuake** in the main OASIS repo:

`C:\Source\OASIS-master\OASIS Omniverse\OQuake\WINDOWS_INTEGRATION.md`

That guide covers building the STAR API wrapper, copying files, and engine integration steps.

## Cross-game key mapping

| Door (game) | Keys that open it |
|-------------|--------------------|
| OQuake silver | silver_key, ODOOM red_keycard, ODOOM skull_key |
| OQuake gold   | gold_key, ODOOM blue_keycard, ODOOM yellow_keycard |
| ODOOM red     | red_keycard, OQuake silver_key |
| ODOOM blue/yellow | blue_keycard, yellow_keycard, OQuake gold_key |
