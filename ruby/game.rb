#!/usr/bin/env ruby

# Brian Anderson - brian@bitbyteyum.com
# Game of Life

# A nice way to print the state of the game....
class PrettyPrintField
  def self.print(state)
    state.each do |row|
      puts row.inspect
    end
  end
  def self.printAt(state,x,y)
    at_y = 0
    state.each do |column|
      at_x = 0  
      column.each do |element|
        if (at_y == y && at_x == x)
          Kernel::print("["+element.to_s+"]")
        else
          Kernel::print(" " + element.to_s + " ")
        end
        at_x+=1
      end

      puts
      at_y+=1
    end

  end
end


# The GAME OF LIFE
# ================================================================
# Public interface...
#
# new(width, height, seed)
# ------------------------
#     Creates a new game of life object with optional parameters
#       width & height  |  integers and optional
#       seed            |  Seed for random number generator
#
# evolve
# ------
#     Steps the game board through one iteration following the rules
#     Returns a game board height x width 2d integer array, 
#       where 1 is alive, 0 is dead
#
# state=[][] (a 2d int array of height x width)
# ----------
#       Give it a board game a game board height x width 2d integer array, 
#       where 1 is alive, 0 is dead
#
class GameOfLife
  # Width     the width of the playing field
  # Height    the height of the playing field
  # Seed      an integer to give to the random number generator
  def initialize(width=10,height=10,seed=rand(1000))
    @width = width
    @height = height
    initializeFieldWithSeed(seed)
  end
  
  # This will walk through the rules of the game of life one step
  # and return the current state of the life
  # The evolve methods should return: an array of arrays
  #   - which represents an array of rows. Where there's life 
  #   (a live cell), there's a 1, 
  #   where there's no life (a dead cell), there's a 0.
  def evolve
    new_field = []
    @height.times do |y|
      new_field << []
      @width.times do |x|
        
        neighbors_count = numberOfNeighborsFor(x,y)

        # -----------------------------------------------------------------
        # any life cell < 2 neighbours dies
        # any life cell > 3 neighbours dies
        if(neighbors_count < 2 || neighbors_count > 3)
          new_field[y][x] = 0
          
        # -----------------------------------------------------------------
        # any dead cell with exactly 3 live neighbours becomes a live cell
        elsif(neighbors_count == 3 && @field[y][x] == 0)
          new_field[y][x] = 1
        
        # -----------------------------------------------------------------
        # There is no change:
        #   any live cell with 2 or 3 neighbours lives to next generation
        #   or the cell stays dead
        else
          # Copy values to the field
          new_field[y][x] = @field[y][x];
        end
      end
    end
    @field = new_field
    return @field
  end
  
  # A way to set the initial state of the game of life
  def state=(twoDimensionalArray)
    @height = twoDimensionalArray.size
    @width = twoDimensionalArray[0].size
    @field = twoDimensionalArray
  end
  
  protected
  
  # Neighbors include wrap around (Yay modulus operator)
  def numberOfNeighborsFor(x,y)
    neighbors = 0;
    [-1,0,1].each do |x_offset|
      [-1,0,1].each do |y_offset|
        if (!(x_offset == 0 && y_offset == 0))
          neighbors+= @field[((y+y_offset) % @height)][((x+x_offset) % @width)];
        end
      end
    end
    return neighbors
  end
  
  # Build the game board
  def initializeFieldWithSeed(seed)
    @seed = seed
    @field = []
    srand(seed) # the Ruby 1.8.7 way
    @height.times do |y|
      row = []
      @width.times do |x|
        row << rand(2) # ruby 1.8.7 way
      end
      @field << row
    end    
  end
end

# life = GameOfLife.new(10,10,2)
# # life.state=[[0,0,0,0,0],
# #             [0,1,1,0,0],
# #             [0,0,0,0,0]]
# game_state = [[1,0,0],[1,1,0],[0,0,0]]
# life.state=game_state
# PrettyPrintField.print(game_state)
# puts '-'
# PrettyPrintField.print(life.evolve)

