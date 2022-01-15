## COMP4300
Building a custom 2D game engine following the public lectures of *Introduction to Game Programming (C++)* [course](https://www.youtube.com/watch?v=LpEdZbUdDe4&list=PL_xRyXins848jkwC9Coy7B4N5XTOnQZzz) of the Memorial University of Newfoundland, using [SFML](https://www.sfml-dev.org/index.php).

Assignments:
- [assignment 02](https://github.com/balazs97feher/COMP4300/tree/assignment-02)
- [assignment 03](https://github.com/balazs97feher/COMP4300/tree/assignment-03)


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