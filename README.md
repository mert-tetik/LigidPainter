# LigidPainter

<img src="https://raw.githubusercontent.com/mert-tetik/LigidPainter/main/LigidPainter/Main/src/.gh_resources/logo-1080x.png" width="200" alt="Ligid Painter Logo"><br><br>

**LigidPainter**  is a free 3D painting software.

## Notes
   This software is not in usable state yet.
   
## Building
glfw3.lib , freetype.lib & freetype.dll files are required.

- After cloning the repository
    Create a new folder named "lib" into the ./LigidPainter/Main/thirdparty/
    Throw glfw3.lib & freetype.lib into that lib folder
    freetype.dll has to be in the same folder as the executable (Ligid.exe)

- Run following commands to initialize and build cmake project
```
    cmake -S . -B build/
    cmake --build build/
```