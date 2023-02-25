# LigidPainter

<img src="https://raw.githubusercontent.com/mert-tetik/LigidPainter/main/LigidPainter/Main/src/.gh_resources/logo-1080x.png" width="200" alt="Ligid Painter Logo"><br><br>

**LigidPainter**  is a free 3D painting software.

<br><br><img src="https://raw.githubusercontent.com/mert-tetik/LigidPainter/main/LigidPainter/Main/src/.gh_resources/lpss.jpg" width="600" height="337" alt="LigidPainterSS"><br><br>

## Notes
Official website : https://www.ligidtools.com/ligidpainter

## Building
glfw3, freetype, assimp libraries are required.

glfw : https://www.glfw.org/download
freetype : https://freetype.org/download.html
assimp : https://github.com/assimp/assimp

- After cloning the repository
    Create a new folder named "lib" into the ./LigidPainter/Main/thirdparty/
    Throw glfw3.lib & freetype.lib into that lib folder
    freetype.dll has to be in the same folder as the executable (Ligid.exe)

- Run following commands to initialize and build cmake project
```
    cmake -S . -B build/
    cmake --build build/
```

## Shortcuts
    Left CTRL + Z = Undo painting
    Left CTRL + X = change invert checkbox's state
    Left CTRL + H = hide or show the texture demonstrator
    Left CTRL + Q + scroll = change brush size range bar value
    Left CTRL + W + scroll = change brush blur range bar value
    Left CTRL + E + scroll = change brush rotation range bar value
    Left CTRL + R + scroll = change brush opacity range bar value
    Left CTRL + T + scroll = change brush spacing range bar value
    Left CTRL + Y + scroll = change brush borders range bar value
    Left CTRL + B = switch painting fill between quality around 1 to 10 
    Left CTRL + TAB + Q = Switch to model panel
    Left CTRL + TAB + W = Switch to texture panel
    Left CTRL + TAB + R = Switch to export panel
    Left CTRL + TAB + T = Switch to painting panel

    While message box is active
    ESC = Negative (Close the message box)
    ENTER = Positive (Exit the app if closing message box is active) 

    Node Shortcuts
    G = Move the selected nodes
    X = Delete the selected nodes
    Left CTRL + Left Mouse Button = Node connection scissor