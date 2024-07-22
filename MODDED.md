(anything that isn't in the future category that says 'will' or 'will be' or 'planned' is now in the source, so it is now: 'has', 'is', etc, etc).

# What's this mod?
An enhanced version of the the port of the game Cro-Mag Rally.
Notably, there will be new features and enhanced old ones, some of which will be committed to this repository soon (they aren't github ready yet).

# What features are there?
Enhanced old features and brand new ones (based on racing games of it's time and newer ones). 
Since we're working with C, some things aren't possible without extra libraries and goodies that may not be compatible with Cro-Mag Rally's specific libraries it already has
(coming from Java background, so there will be mistakes, none that mess with memory however).

## Here's a list of the old enhanced features:

- CPUs now cheat! CPUs are fairly dumb, so to give them some edge, they can turn invisible, have random boosts at any time, and their cars are enhanced randomly at the start, plus more little things.
- CPUs now fire throwable items slower or faster depending on RNG.
- Nitro is now dependent on placement in the race: You get less nitro time if your in first, versus if your in last place.
- Bottle Rockets/Roman Candles' items now give you more per collection, as they are the most effective against CPUs and the player (15-20ish).
- Bone Bombs, Oil, and the other non-candle/rocket items give you a random amount between 2-3, or 5-7, etc.-etc., depending on RNG.
- Oil now flies further from players and CPUs.
- Homing Pigeons fly faster and start up in the air more.
- Bottle Rockets fly faster.
- Torpedos used in non-water tracks don't display bubbles or play looping sounds (as they are not loaded, which caused errors, fixed the error, also unintendedly fixes the immersion a bit).
- Submarine turns into a plane in normal levels (doesn't look like one, but does remove bubble particles) and if you collect anything but nitro, the item is a SUPER BURST, giving you a 1 of each; Roman Candles, Bottle Rockets, and Torpedos, which can hit CPUs in non-sub vehicles.
- Terrain is coord stretched horizontally (not vertically to not ruin the vertical stretch physics feature), and CPUs can drive on it no problem. The track is now wider and longer.
- Nitro now boosts you more if your in a Submarine on a non-Atlantis track, and Sticky Tires/Super Suspension give you a tiny boost of speed (non-nitro speed, depending on the number of seconds shown).
- Some previously static objects now dance or move to make the scene a little more active, but not too distracting. For example, the trees now stretch up and down smoothly to look like they are moving in the wind.

## Here's a list of the brand new features:
- Zapped Player State (has no proper icon yet); when zapped, you will turn small and move much slower than your usual speed. When the timer runs out, you will regain your original speed and size.
- Night Mode (WIP); selected tracks will have a night time/sunset lighting and coloring option (and sometimes level changes to objects). The skybox, terrain and objects will be shaded and have an alternative appearance (it is currently working but is not synced nor is 'night' in the sense of lighting/dark shadows, as the custom C code and the OpenGL-like system doesn't have good support for dynamic lights, that are needed for full effect, and such).
- Fog is enabled in particular levels depending on how good fog looks in the level (was disabled entirely in the whole game)
- Fast Music on Lap 3+ (will change if others are in 3rd lap as well)
- Completed race music (will use title theme but altered a bit, WIP)
- Zapper Item (collected only from Arrowheads in non-tournament modes) (zaps only players/CPUs who did not fire the zapper for 5 seconds, if user who fired zapper is already zapped, their zapper timer will be reset to 0)

# List Of Potential Fixes/Technical Changes:
- Unknown object types will be corrected if possible, if not, the objects will still cause errors, due to there being no replacement for them (remember that 'objects' is a relative term as there are no real objects in C)
- Odd behaviors (that seemed like bugs) in the original game will be changed to be more intentional/removed/fixed rather than a bug (such as clipping when not correct, or odd hitboxes)
- Crete finish line increased size and hitbox (matched up using debugger features, hitbox is not blocking more road, rather resized to new model scale)
- Objects that weren't lit using lighting effects in OpenGL now use lighting effects when appropriate (such as objects in day mode not using light effects vs night mode where they do need light effects).
- Hitbox tweaks to 'token', a.k.a Arrowhead used in tournament gamemode.
- More sound channels as original amount was too low for certain circumstances, cutting out sounds that weren't supposed to be cut out at all (20 -> 60 channels).
- Player ID's are assigned at the start of a race, so this value is stored within the player's info so that more complex features can be created for players.
  
# List of potential future changes, if possible:
- Potential for objects to be more in line with C++? (there is some code already from the port that uses real classes and object-like stuff).
- Levels could be loaded from non-.ter (these can contain terrain and textures according to some research) and non-.ter.rsrc (other terrain resources, or Macintosh Resource Forks type) files.
- BG3D models/skeletons/animations can be exported using scripts (and are usually converted into multi-model files with some attributes and custom data, perhaps the original files are packed as well), but not imported as there is no other known way to make them without Maya 6 and a custom plugin (according to Pangea Software Game Making Book). Perhaps add support for .obj or .dae files, as they are fairly old enough to perhaps be converted on the fly to bg3d... unsure, needs more testing.
- Some way to load all game textures outside of atlases, as atlases are good for old computers and compression, but bad as they are hardly expandable. If a computer has plenty of space, shouldn't an atlas be *made after* the textures are *all identified and can be referenced*? (look into other games and how they stitch individual textures in one singular atlas for the game to use).
- Support for more compressed/alternative formats (game supports PNGs/JPGS for sprites/screen bgs, and AIFFs for all sfx and music already, which is good, perhaps expand this support to other formats/find out what formats are in BG3D files for their textures natively? (TGAs were the converted texture format using python scripts)).

Note:
If you want to help make the mod better, please report anything related to: "Bad Races, Difficulty changes, Options Within Bounds Of OpenGL For C", "Bugs/Crashes", "Strange behaviors".
**Do not report mod suggestions/issues/bugs to the original GitHub that Jorio has made, as that port is for official changes only, which may propagate if they add features or fix critical issues, or optimize the game more.**
