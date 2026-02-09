# Windows Integration Guide for OQuake (Quake + STAR API)

This guide is for integrating the OASIS STAR API into your Quake fork so that **OQuake** can share keys with **ODOOM**: keys collected in Doom open doors in Quake and vice versa.

**Quake source (QuakeC):** `C:\Source\quake-rerelease-qc`  
**OQuake integration files:** This folder (`OASIS Omniverse\OQuake`).

**One-click script (same as ODOOM/UZDoom):** From `C:\Source\OASIS-master` run:

```bat
"OASIS Omniverse\OQuake\BUILD_OQUAKE.bat"
```

This builds the STAR API if needed, then copies all OQuake files and star_api into `C:\Source\quake-rerelease-qc`. Use `BUILD_OQUAKE.bat run` to launch your engine after setting `QUAKE_ENGINE_EXE` in the script.

## Game data (fixing "couldn't load gfx.wad")

vkQuake needs the **Quake game data**. The engine uses a **basedir** (default: the folder containing `vkquake.exe`) and expects:

- An **id1** subdirectory containing **pak0.pak** and **pak1.pak** (from the original Quake or a re-release).
- **gfx.wad** in the basedir or in **id1** (depending on your data layout).

**Option A – Use a separate game directory (recommended)**  
Create a folder for game data (e.g. `C:\Quake`). Inside it put:

- **id1\pak0.pak**, **id1\pak1.pak**
- **gfx.wad** (in the same folder as the exe, or in **id1**)

Then run vkQuake with:

```bat
vkquake.exe -basedir C:\Quake
```

**Option B – Copy game data next to the exe**  
From your Quake install (Steam, GOG, or original), copy into the vkQuake output folder (e.g. `Build-vkQuake\x64\Release`):

- The **id1** folder (with pak0.pak, pak1.pak)
- **gfx.wad** (if your Quake install has it in the root, copy it into the Release folder or into id1)

You must own Quake to use the game data; get it from [Steam](https://store.steampowered.com/app/2310/Quake/), [GOG](https://www.gog.com/game/quake_the_offering), or similar.

## Prerequisites

1. **Visual Studio** (2019 or later) with C++ development tools
2. **CMake** (3.10 or later)
3. **Quake engine source** that builds the executable which runs the QuakeC from `quake-rerelease-qc`
4. **STAR API credentials** (same as ODOOM: SSO or API key)

## Step 1: Build the Native Wrapper

Same as ODOOM. From OASIS root:

```powershell
cd C:\Source\OASIS-master
cd "OASIS Omniverse\NativeWrapper"
mkdir build -Force
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

Output: `OASIS Omniverse\NativeWrapper\build\Release\star_api.lib` and `star_api.dll`.

## Step 2: Set Environment Variables

Same as ODOOM (see `Doom\WINDOWS_INTEGRATION.md` Step 2). Use `STAR_USERNAME`/`STAR_PASSWORD` or `STAR_API_KEY`/`STAR_AVATAR_ID`.

## Step 3: Copy OQuake Integration Files to Your Quake Tree

If your **engine** source lives with the QuakeC (e.g. a single repo):

```powershell
cd C:\Source\OASIS-master

# Copy OQuake integration and STAR API into Quake source
$QUAKE_SRC = "C:\Source\quake-rerelease-qc"
Copy-Item "OASIS Omniverse\OQuake\oquake_star_integration.c" $QUAKE_SRC
Copy-Item "OASIS Omniverse\OQuake\oquake_star_integration.h" $QUAKE_SRC
Copy-Item "OASIS Omniverse\OQuake\oquake_version.h" $QUAKE_SRC
Copy-Item "OASIS Omniverse\NativeWrapper\star_api.h" $QUAKE_SRC
Copy-Item "OASIS Omniverse\NativeWrapper\build\Release\star_api.dll" $QUAKE_SRC
Copy-Item "OASIS Omniverse\NativeWrapper\build\Release\star_api.lib" $QUAKE_SRC
```

If the **engine** is in a different directory, copy the same files into the **engine** project instead of `quake-rerelease-qc`.

## Step 4: Engine Modifications (C Code)

QuakeC cannot call C functions directly. The **engine** must:

1. Call `OQuake_STAR_Init()` at startup and `OQuake_STAR_Cleanup()` at shutdown.
2. When the player picks up a key (in the engine’s item-touch handler or via a known key entity), call:
   - `OQuake_STAR_OnKeyPickup("silver_key")` for silver key (IT_KEY1)
   - `OQuake_STAR_OnKeyPickup("gold_key")` for gold key (IT_KEY2)
3. When the player touches a key door and **does not** have the required key locally, call:
   - `OQuake_STAR_CheckDoorAccess(door_targetname, "silver_key")` or `"gold_key"`.
   If it returns 1, open the door and consume the key (or use a one-time use as in STAR API).

Include in the engine (e.g. `host.c` or main init):

```c
#include "oquake_star_integration.h"
```

In your init (e.g. `Host_Init`):

```c
OQuake_STAR_Init();
```

In your shutdown:

```c
OQuake_STAR_Cleanup();
```

In the code path that handles **key pickup** (when the engine detects that the player touched `item_key1` / `item_key2` or equivalent):

```c
// After giving the player IT_KEY1 / IT_KEY2
OQuake_STAR_OnKeyPickup("silver_key");   // for IT_KEY1
OQuake_STAR_OnKeyPickup("gold_key");     // for IT_KEY2
```

In the code path that handles **key door touch** (when the player doesn’t have the required key):

```c
// required_key is "silver_key" or "gold_key" depending on door
if (OQuake_STAR_CheckDoorAccess(door_targetname, required_key)) {
    // Open door (same as when player had local key)
    door_fire();
    return;
}
```

## Step 5: QuakeC and engine builtins (C:\Source\quake-rerelease-qc)

The QuakeC in `quake-rerelease-qc` already calls two **extension builtins** that the engine must implement:

- **OQuake_OnKeyPickup(keyname)** – called from `items.qc` when the player picks up silver or gold key.
- **OQuake_CheckDoorAccess(doorname, requiredkey)** – called from `doors.qc` when the player touches a key door without the local key; returns 1 to open with cross-game key.

In `defs.qc` they are declared as `#0:ex_OQuake_OnKeyPickup` and `#0:ex_OQuake_CheckDoorAccess`. Your engine must register these extension builtins and call `OQuake_STAR_OnKeyPickup` and `OQuake_STAR_CheckDoorAccess` from `oquake_star_integration.c`. See the example file in the Quake tree:

**`C:\Source\quake-rerelease-qc\engine_oquake_hooks.c.example`**

That file shows the C wrappers and how to wire them into your engine’s extension builtin system.

The game logic in `quakec\doors.qc` and `quakec\items.qc` already contains the calls; the engine must hook into:

- **Key pickup:** Where the engine gives the player `IT_KEY1` or `IT_KEY2` (e.g. after `key_touch` in QuakeC triggers or when the engine handles the same).
- **Door touch:** Where the engine or QuakeC reports “player touched door and doesn’t have key.” The engine can then call `OQuake_STAR_CheckDoorAccess` and, if it returns 1, trigger the door open.

So no change to QuakeC is strictly required if the engine adds the hooks above. If you prefer to keep a marker in QuakeC (e.g. comments) for where the engine should call the STAR layer, see `C:\Source\quake-rerelease-qc\OQUAKE_INTEGRATION.md`.

## Step 6: Build System

- Add `oquake_star_integration.c` to the engine project.
- Include path to the directory containing `oquake_star_integration.h` and `star_api.h`.
- Link `star_api.lib` and `winhttp.lib` (Windows).
- Ensure `star_api.dll` is next to the built executable (or on PATH).

## Step 7: Branding (Optional)

Use `oquake_version.h`: `OQUAKE_TITLE` for window title so the game shows as **OQuake 1.0 (Build 1)**.

## Cross-Game Key Mapping

| OQuake door  | Keys that open it (local + cross-game)        |
|-------------|------------------------------------------------|
| Silver key  | OQuake silver_key, ODOOM red_keycard, skull_key |
| Gold key    | OQuake gold_key, ODOOM blue_keycard, yellow_keycard |

| ODOOM door  | Keys that open it (local + cross-game)        |
|-------------|------------------------------------------------|
| Red         | ODOOM red_keycard, OQuake silver_key           |
| Blue / Yellow | ODOOM blue/yellow keycard, OQuake gold_key   |

## Testing

1. Run OQuake with STAR env vars set. Console should show:  
   `OQuake STAR API: Authenticated via SSO. Cross-game features enabled.`
2. Pick up silver key in OQuake → console: `OQuake STAR API: Added silver_key to cross-game inventory.`
3. Run ODOOM, find a red keycard door. Open it with the key you got in OQuake (if mapping is applied in Doom).
4. In ODOOM pick up red keycard, then in OQuake open a silver door without the local key → door should open via cross-game key.

## Troubleshooting

- **star_api.lib/dll not found:** Build NativeWrapper and copy the DLL next to the exe.
- **No cross-game keys:** Ensure STAR_USERNAME/STAR_PASSWORD or STAR_API_KEY/STAR_AVATAR_ID are set and init succeeds.
- **Doors don’t open with other game’s key:** Ensure the engine calls `OQuake_STAR_CheckDoorAccess` when the player lacks the local key and passes the correct `required_key` (`"silver_key"` or `"gold_key"`).

## Support

- [ODOOM Windows Integration](../Doom/WINDOWS_INTEGRATION.md)
- [Main Integration Guide](../INTEGRATION_GUIDE.md)
