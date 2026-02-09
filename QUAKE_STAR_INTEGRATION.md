# Quake STAR API Integration

## Overview

Quake uses QuakeC (an interpreted language), so integration requires:
1. Native C bridge functions (in quake_star_integration.c)
2. QuakeC modifications to call the bridge functions
3. Engine modifications to expose bridge functions to QuakeC

## Integration Points

### 1. Key Pickup (items.qc)

In `item_key1` and `item_key2` functions, add STAR API tracking:

```qc
void() item_key1 =
{
    // ... existing code ...
    
    // OASIS STAR API: Track key pickup
    // Note: Requires native bridge function QuakeC_OnKeyPickup()
    // QuakeC_OnKeyPickup("silver_key");
    
    self.items = IT_KEY1;
    // ... rest of code ...
};

void() item_key2 =
{
    // ... existing code ...
    
    // OASIS STAR API: Track key pickup
    // QuakeC_OnKeyPickup("gold_key");
    
    self.items = IT_KEY2;
    // ... rest of code ...
};
```

### 2. Door Access (doors.qc)

In `door_touch` function, add cross-game inventory check:

```qc
void() door_touch =
{
    // ... existing code ...
    
    // key door stuff
    if (!self.items)
        return;

    // Check local inventory first
    if ( (self.items & other.items) != self.items )
    {
        // OASIS STAR API: Check cross-game inventory
        // Note: Requires native bridge function QuakeC_CheckDoorAccess()
        // if (self.owner.items == IT_KEY1) {
        //     if (QuakeC_CheckDoorAccess(self.targetname, "silver_key"))
        //         door_fire();  // Door opened with cross-game key!
        //         return;
        //     }
        // }
        // Similar for IT_KEY2 (gold_key)
        
        // ... existing "need key" messages ...
        return;
    }
    
    // ... rest of code ...
};
```

## Native Bridge Functions

The native C bridge functions are in `quake_star_integration.c`. These need to be exposed to QuakeC via the engine.

### Required Engine Modifications

The Quake engine needs to register these functions with the QuakeC VM:

```c
// In engine code (e.g., pr_cmds.c or similar)
PR_RegisterFunction("QuakeC_OnKeyPickup", QuakeC_OnKeyPickup);
PR_RegisterFunction("QuakeC_CheckDoorAccess", QuakeC_CheckDoorAccess);
PR_RegisterFunction("QuakeC_OnItemPickup", QuakeC_OnItemPickup);
```

### Bridge Function Signatures

```c
// Called from QuakeC when key is picked up
void QuakeC_OnKeyPickup(const char* key_name);

// Called from QuakeC to check door access
int QuakeC_CheckDoorAccess(const char* door_name, const char* required_key);

// Called from QuakeC when item is picked up
void QuakeC_OnItemPickup(const char* item_name, const char* item_desc);
```

## Implementation Steps

1. **Build Native Wrapper**: Build `star_api` library from OASIS project
2. **Add Bridge Functions**: Add bridge functions to quake_star_integration.c
3. **Modify Engine**: Register bridge functions with QuakeC VM
4. **Modify QuakeC**: Add calls to bridge functions in items.qc and doors.qc
5. **Build Quake**: Link star_api library and rebuild

## Testing

1. Start Quake with STAR API integration
2. Pick up silver key → Should see "STAR API: Added silver_key to cross-game inventory"
3. Start DOOM → Pick up red keycard
4. Return to Quake → Try to open door requiring silver key
5. Door should open using DOOM's red keycard (if mapped)!

## Notes

- QuakeC is interpreted, so native functions must be registered with the engine
- The exact method depends on your Quake engine implementation
- Some engines support direct native function calls from QuakeC
- Check your engine's documentation for native function registration





