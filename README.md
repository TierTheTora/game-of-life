# Conways Game of Life (Linux and Windows)
## Features
 - Compatible for Windows and Linux
 - Multiple console colours and formatting (For linux)
 - 20x20 Cell board
 - Console like User Interface
 - 2D Cell placing system
 - Script file reading
## Commands
 - \+ <int>, <int>    : Place a cell on the board
 - \- <int>, <int>    : Remove a cell on the board
 - board             : Display the game board
 - run               : Run the simulation
 - read              : Read a script file
## Rules
1. Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
   - ([source](https://rustwasm.github.io/book/game-of-life/rules.html))
  
## Symbols
- #: Alive cell
- .: Dead cell
