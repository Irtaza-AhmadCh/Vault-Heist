# Running the game

The game entry point is [game/main.cpp](../game/main.cpp). Open that file in
VS Code and press **F5** to build and start the game, or **Ctrl+F5** to
start without debugging. The configured launch task compiles the game
sources, uses the SFML 2.6.2 x64 headers and libraries from
`docs/SFML-2.6.2-x64/SFML-2.6.2`, and adds its `bin` folder to the runtime
`PATH`.

The build output is `main.exe` in the repository root. The launch
configuration sets the repository root as the working directory so the
game can load the `assets` and `levels` directories. No batch file is
needed.
