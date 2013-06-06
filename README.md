#Game Of Lives

This is my playground to experiment with new programming languages. Building the Conway's [game of life](http://en.wikipedia.org/wiki/Conway's_Game_of_Life). A [Cellular automaton](http://en.wikipedia.org/wiki/Cellular_automaton) simulation devised in the 1970s. 

## Rules

The rules of the game of life are trivial. I borrowed these from [elisehuard](https://github.com/elisehuard/game_of_life)

You have a grid of cells in 2 dimensions. Each cell has 2 possible states, alive or dead. Each cell has 8 neighbors: above, below, left, right, and the 4 diagonals.

* any life cell < 2 neighbors dies
* any life cell > 3 neighbors dies
* any live cell with 2 or 3 neighbors lives to next generation
* any dead cell with exactly 3 live neighbors becomes a live cell

_edges of game: just pretend that the board is folded onto itself, and the edges touch each other. If that's too complicated, you can work without that assumption._

## Languages

1. Ruby &#x2713;
2. C
3. C++
4. Javascript
5. ...?