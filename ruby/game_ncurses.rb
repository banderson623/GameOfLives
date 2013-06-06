# This N curses runner is based on https://github.com/elisehuard/game_of_life

require 'rubygems'
require 'ffi-ncurses'

class LifeNcurses

  # spaces from the border of the terminal
  MARGIN = 2
  include FFI::NCurses

  def initialize(game_of_life,iterations=100)
    @stdscr = initscr
    cbreak
    
    state = last_state = []
    (1..iterations).each do |generation|
      clear
      display_title(generation)
      state = game_of_life.evolve
      show state
      if(state == last_state) 
        endwin
      end
      last_state = state
    end 
  ensure
    endwin
  end

  def show(state)
    state.each_with_index do |row,row_index|
      row.each_with_index do |col, col_index|
        mvwaddstr @stdscr, row_index+MARGIN, col_index+MARGIN, '*' if state[row_index][col_index] == 1
      end
    end
    refresh
    sleep 0.01
  end

  def display_title(generation)
    mvwaddstr @stdscr, 0, 1, "Game of life: Generation #{generation}"
  end

end