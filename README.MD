
## Program Specification

The game must have the following features:

### Player:
- [x] The player is represented by a shape which is defined in the config file.
- [ ] The player must spawn in the center of the screen at the beginning of the game, and after it dies. (collides with an enemy)
- [x] The player moves by a speed read from the config file in these directions:
  - Up: 'W' key
  - Left: 'A' key
  - Down: 'S' key
  - Right: 'D' key
- [x] The player is confined to move only within the bounds of the window.
- [x] The player will shoot a bullet toward the mouse pointer when the left mouse button is clicked. The speed, size and lifespan of the bullets are read from the config file.

### Special Ability:
Come up with a 'special move' which is fired by the player when the right mouse button is clicked. This special ability must:
- [ ] Spawn multiple entities. (bullets, etc)
- [ ] Entities have some unique graphic associated with them.
- [ ] A unique game mechanic is introduced via a new component.
- [ ] A 'cooldown timer' must be implemented for the special weapon.

The properties of the special move are not in the config file.

### Enemies:
- [x] Enemies will spawn in a random location on the creen every X frames, where X is defined in the config file.
- [x] Enemies must not overlap the sides of the screen at the time of spawning.
- [x] Enemies' shapes have random number of vertices, between a given minimum and maximum value, specified in the config file.
- [x] Enemy shape radius will be specified in the config file.
- [x] Enemies will be given a random color upon spawning.
- [x] Enemies will be given a random speed upon spawning, between a minimum and maximum value, specified in the config file.
- [x] When an enemy reaches the edge of the window, it should bounce off in the opposite direction at the same speed.
- [x] When (large) enemies collide with a bullet or a player, they are destroyed, and N small enemies spawn in its place, where N is the number of vertices of the original enemy. Each small enemy must have the same number of vertices and color of
the original enemy. These small entities travel outward at angles at fixed intervals equal to (360 / vertices).

### Score:
- [x] Each time an enemy spawns, it is given a score component of N*100, where N is the number of vertices it has. Small enemies get double this value.
- [x] If a player bullet kills an enemy, the game score is increased by the score component of the enemy killed.
- [x] The score should be displayed with the font specified by the config file in the top-left corner of the screen.

### Drawing:
- [x] In the render system, all entities should be given a slow rotation.
- [x] Any Entity with a lifespan is currently alive, it should have its color alpha channel set to a ratio depending on how long it has left to live. For example, if an Entity has a 100 frame life span, and it has been alive for 50 frames, its alpha value should be set to 0.5 * 255. The alpha should go from 255 when it is first spwaned, to 0 on the last frame it is alive.

### Miscellaneous:
- [x] The 'P' key should pause the game.
- [x] The 'ESC' key should close the game.

### Configuration File:
**Window W H FL FS**
  - FL - frame limit
  - FS - full screen mode (0 or 1)
  
**Font F S R G B**
  - F - file
  - S - size
  - R G B - color components
  
**Player SR CR S FR FG FB OR OG OB OT V**
  - SR - shape radius
  - CR - collision radius
  - S - speed
  - FR FG FB - fill color components
  - OR OG OB - outline color components
  - OT - outline thickness
  - V - number of vertices
  
**Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI**
  - SMIN, SMAX - minimum and maximum speed
  - VMIN VMAX - minimum and maximum number of vertices
  - L - small enemy lifespan in frames
  - SI - spawn interval in frames
  
**Bullet SR CR S FR FG FB OR OG OB OT V L**

