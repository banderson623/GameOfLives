#!/usr/bin/env ruby

# Ruby test case from https://github.com/elisehuard/game_of_life

require File.join(File.dirname(__FILE__), 'game')
require 'rubygems'
require 'test/unit'

class GameOfLifeTest < Test::Unit::TestCase

  def setup
    @game = GameOfLife.new(3)
  end

  def test_should_kill_with_no_neighbours
    @game.state = [[1,0,0],[0,0,0],[0,0,0]]
    after = @game.evolve
    assert_equal after[0][0], 0
  end

  def test_should_kill_with_just_one_neighbour
    @game.state = [[0,0,0],[1,0,0],[1,0,0]]
    after = @game.evolve
    assert_equal after[1][0], 0
    assert_equal after[2][0], 0
  end

  def test_should_kill_with_more_than_3_neighbours
    @game.state = [[1,1,1],[1,1,1],[1,1,1]]
    after = @game.evolve
    assert_equal after, [[0,0,0],[0,0,0],[0,0,0]]
  end

  def test_should_give_birth_if_3_neighbours
    @game.state = [[1,0,0],[1,1,0],[0,0,0]]
    after = @game.evolve
    assert_equal after, [[1,1,1],[1,1,1],[1,1,1]]
  end

end