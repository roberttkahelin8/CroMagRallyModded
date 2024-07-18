# What's this mod?
An enhanced version of the the port of the game Cro-Mag Rally.
Notably, there will be new features and enhanced old ones, some of which will be committed to this repository soon (they aren't github ready yet).

# What features are there?
Enhanced old features and brand new ones (based on racing games of it's time and newer ones). 
Since we're working with C, some things aren't possible without extra libraries and goodies that may not be compatible with Cro-Mag Rally's specific libraries it already has
(coming from Java background, so there will be mistakes, none that mess with memory however).

## Here's a list of the old enhanced features:

- CPUs now cheat! CPUs are fairly dumb, so to give them some edge, they can turn invisible, have random boosts at any time, and their cars are enhanced randomly at the start, plus more little things.
- Nitro is now dependent on placement in the race: You get less nitro time if your in first, versus if your in last place.
- Bottle Rockets/Roman Candles' items now give you more per collection, as they are the most effective against CPUs and the player.
- Bone Bombs, Oil, and the other non-candle/rocket items give you a random amount between 2-3, or 5-7, etc.-etc., depending on RNG.
- Submarine turns into a plane in normal levels (doesn't look like one, but does remove bubble particles) and if you collect anything but nitro, the item is a SUPER BURST, giving you a 1 of each; Roman Candles, Bottle Rockets, and Torpedos, which can hit CPUs in non-sub vehicles.
- Terrain is coord stretched horizontally (not vertically to not ruin the vertical stretch physics feature), and CPUs can drive on it no problem. The track is now wider and longer.
- Nitro now boosts you more if your in a Submarine on a non-Atlantis track, and Sticky Tires/Super Suspension give you a tiny boost of speed (non-nitro speed, depending on the number of seconds shown).
- Some previously static objects now dance or move to make the scene a little more active, but not too distracting. For example, the trees now stretch up and down smoothly to look like they are moving in the wind.

## Here's a list of the brand new features:
- Zapped Player State (has no proper icon yet); when zapped, you will turn small and move much slower than your usual speed. When the timer runs out, you will regain your original speed and size.
- Night Mode (WIP); selected tracks will have a night time lighting and coloring option. The skybox, terrain and objects will be shaded and have an alternative appearance (it is currently working but is not synced nor is 'night' in the sense of lighting/dark shadows, as the custom C code and the OpenGL-like system doesn't have good support for dynamic lights, that are needed for full effect, and such).
- Fog is enabled in particular levels depending on how good fog looks in the level.
