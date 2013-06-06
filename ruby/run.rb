#!/usr/bin/env ruby

require 'game.rb'
require 'game_ncurses.rb'

# life = GameOfLife.new(30,30,20)
life = GameOfLife.new(100,30,1)

LifeNcurses.new(life,1000)