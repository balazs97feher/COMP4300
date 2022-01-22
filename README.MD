## Program Specification

The game must have the following features:

### Assets:
- [ ] Entities in the game will be rendered using various Textures and Animations, which we will be calling Asssets (along with Fonts).
- [ ] Assets are loaded once at the beginning of the program and stored in the Assets class, which is stored by the GameEngine class.
- [ ] All assets are defined in assets.txt, with the syntax defined below.

Note: All entity positions denote the center of their rectengular sprite and bounding box (if it has one).

### Player:
- [ ] The player entity in the game is represented by Megaman, which has several different Animations: Stand, Run and Air. The state of the player is to be determined and assigned the appropriate Animation.
- [ ] The player moves with the following controls:
  - Jump: 'W' key
  - Left: 'A' key
  - Right: 'D' key
  - Shoot: Space key
- [ ] The player can move left and right, shoot any time, including in the air.
- [ ] The player can only jump, if it is currently standing on a tile.
- [ ] If the jump key is held, the player should not continuously jump, but instead it should only jump once per button press.
- [ ] If the player lets go of the jump key mid-jump, it should start falling back down immediately.
- [ ] If the player moves left/right, the player's sprite will face in that
direction until the other direction has been pressed.
- [ ] Bullets shot by the player travel in the direction the player is facing.
- [ ] The player collides with 'Tile' entities in the level (see level syntax) and
cannot move through them. The player lands on a Tile entity and stands in place
if it falls on it from above.
- [ ] The player does not collide with 'Dec' (decoration) entities in the level.
- [ ] If the player falls below the bottom of the screen, they respawn at the start.
- [ ] The player should have a Gravity component which constantly accelerates it downward on the screen until it collides with a tile.
- [ ] The player has a maximum speed specified in the Level file (see below) which
it should not exceed in either x or y direction.
- [ ] The player will be given a CBoundingBox of a size specified in the level file.
- [ ] The player's sprite and bounding box are centered on the player's position.

### Animations:
See below for Animation asset specification.
- [ ] Animations are implemented by storing multiple frames inside a texture.
- [ ] The Animation class handles frame advancement based on animation speed. Animation:: update() is to be implemented to properly progress animations.
- [ ] Animation::hasEnded() returns true if an animation has finished its last frame, false otherwise.
- [ ] Animations can be repeating (loop forever) or non-repeating (play once).
- [ ] Any entity with a non-repeating animation should be destroyed once its
Animation's hasEnded() returns true (has finished one cycle).

### Decoration Entities:
- [ ] Decoration entities ('Dec' in a level file) are simply drawn to the screen,
and do not interact with any other entities in the game in any way.
- [ ] Decorations can be given any Animation in the game, but intuitively they
should be reserved for things like clouds, bushes, etc.

### Tiles:
- [ ] Tiles are Entities that define the level geometry and interact with players.
- [ ] Tiles can be given any Animation that is defined in the Assets file.
- [ ] Tiles will be given a CBoundingBox equal to the size of the animation.
- [ ] Tiles have different behavior depending on which Animation they are given. (Brick tiles or question tiles)

### Assets Configuration:
There are four different line types in the Assets file, each of which correspond to a different type of Asset. They are as follows:
- Texture Asset Specification: **Texture N P F**
  - Texture Name N as string
  - Texture File Path P as string
  - Frame Count F as integer (total number of frames (horizontally!) in the texture sheet)
  - e.g. **Texture megaman textures/megaman.png 5**
- Animation Asset Specification: **Animation N T SF F S**
  - Animation Name N as string
  - Texture Name T as string (refers to an existing texture)
  - Start Frame Index as integer (index (starting at 0) of the first frame  of the Animation in the Texture)
  - Frame Count F as integer (number of frames in the Animation)
  - Animation Speed S as integer (number of game frames between animation frames)
  - e.g. **Animation megaman_running megaman 1 4 30**
- Sound Asset Specification: **Sound N P**
  - Sound Name N as string
  - Sound File Path P as string
  - e.g. **Sound jump sounds/jump.wav**
- Font Asset Specification: **Font N P**
  - Font Name N as string
  - Font File Path Р as string
  - e.g. **Font ayar fonts/ayar.ttf**

Note, that texture sheets must have a horizontal layout, where each frame has the same dimensions.

### Level Configuration
Game levels will be specified by a Level file, which will contain a list of entity specifications, one per line. It will also contain a single line which specifies properties of the player in that level. In this way, you can define an entire level in the data file, rather than in programming code. The syntax of the lines of the Level file are as follows.

IMPORTANT NOTE:
All (GX, GY) positions given in the level specification file are given in "grid' coordinates. The 'grid' cells are of size 64 by 64 pixels, and the entity should be positioned such that the bottom left corner of its texture is aligned with the bottom left corner of the given grid coordinate. The grid starts at (0,0) in the bottom-left of the screen, and can be seen by pressing the 'G' key while the game is running.

- Tile Entity Specification: **Tile N GX GY**
  - Animation Name N as string (Animation asset name for this tile)
  - Grid X Pos GX float
  - Grid Y Pos GY float
- Decoration Entity Specification: **Dec NX Y**
  - Animation Name N as string (Animation asset name for this tile)
  - X Position X float
  - Y Position Y float
- Player Specification: **Player GX GY CW CH SX SY SM GY B**
  - Grid X Pos GX float
  - Grid Y Pos GY float
  - BoundingBox width CW float
  - BoundingBox height CH float
  - Left/Right Speed SX float
  - Jump Speed SY float
  - Max Speed SM float
  - Gravity GY float
  - Bullet Animation B as string (Animation asset to use for bullets)

### Miscellaneous:
- [ ] Pressing the 'P' key pauses the game.
- [ ] Pressing the 'T' key toggles drawing textures.
- [ ] Pressing the 'C' key toggles drawing bounding boxes of entities.
- [ ] Pressing the 'G' key toggles drawing of the grid.
- [ ] The 'ESC' key should go 'back' to the Main Menu, or quit if on the Main Menu.