# SDL2_editor
A simple map editor to be used for 2D games written with SDL2 in C++.
## Demo
![demo](example/demo.gif)
## Prerequisites
- [SDL2](https://github.com/libsdl-org/SDL)
- [cvec](https://github.com/broskobandi/cvec.git) (for running the tests)
- [cmake](https://cmake.org/download/) (for building the project)
- [nlohmann/json](https://github.com/nlohmann/json) (for saving into json)
# Installation
```bash
git clone https://github.com/broskobandi/SDL2_editor.git &&
mkdir SDL2_editor/build &&
cd SDL2_editor/build &&
cmake .. &&
make &&
# Optionally run sudo make install
# Then run the created SDL2_editor binary.
```
# Key bindings
- R -> rotate tile.
- F -> flip tile.
- S -> save map. (for now, it simply creates a tiles.json in the current dir)
- Q -> exit editor. 
# Output format
```json
[
    {
        "angle": 0.0,
        "flip": 0,
        "h": 64,
        "path_to_bmp": "<path>/corner.bmp",
        "w": 64,
        "x": 80,
        "y": 0
    },
    {
        "angle": 0.0,
        "flip": 0,
        "h": 64,
        "path_to_bmp": "<path>/wall.bmp",
        "w": 64,
        "x": 144,
        "y": 0
    },
    ...
]
```
