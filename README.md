# Conways Game of Life v2.7.0
## Features
 - Compatible for Windows and Linux
 - Multiple console colours and formatting (For linux)
 - 20x20 Cell board
 - Console like User Interface
 - 2D Cell placing system
 - Script file reading
 - Uses # for comments
 - Optional board wrapping
 - Quit the simulation and return to command mode with 'q' or 'x'
## Commands
 - \+ \<int\>, \<int\>    : Place a cell on the board
 - \- \<int\>, \<int\>    : Remove a cell on the board
 - board             : Display the game board
 - run               : Run the simulation
 - read              : Read a script file
 - help              : Display the help menu
 - speed = \<int\>     : Set game speed (ms)
 - size = \<int\>, \<int\> : Set board size
 - exit / quit       : Quit the program
 - stat              : Get board info
 - step \<int\>        : Step a given amount of generations
 - clear             : Clear the board
 - fill              : Fill the board with alive cells
 - wrap = \<on/off\>      : Turn board wrapping on or off
 - save \<file\>           : Save the board as a file
 - restore            : Restore the board from before the run
 - random \<int\> : Randomize the board
 - pattern \<name\> : Load a predefined pattern
 - mirror : Mirror the board
## Rules
1. Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
   - ([source](https://rustwasm.github.io/book/game-of-life/rules.html))
  
## Symbols
- \#  : Alive cell
- .  : Dead cell
