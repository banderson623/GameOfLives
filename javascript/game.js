// Game of Life in Javascript
// Brian Anderson


var Game = {
    current: null,
    
    // Generates the board
    generate: function(height, width, seed){
        var new_board = Game.Board;
        new_board.height = height;
        new_board.width = width;
        var tiles = [];
        var a_row = [];
        // todo, find out how to seed this
        for(var row = 0; row < height; row++){
            for(var column = 0; column < width; column++){
                // 1 is life exists, 0 no life on the cell
                a_row[column] = Math.ceil(Math.random()*10) % 2
            }
            tiles[row] = a_row;
        }
        // copy, not reference, right?
        new_board.tiles = tiles;
        return new_board;
    },
    
    // set things up
    setup: function(){
        this.current = this.generate(10,10);
    }
    
    
    step: function(){
        this.current = this.current.evolve();
    }
    
    Board: {}
}

Game.Board = {
    // tiles height, integer
    height: 1,
    // tile width, integer
    width: 1,
    tiles: [] // ends up being a 2d array.
    
    // returns a new instance of board
    evolve: function(){
        var new_board = this;
        var number_of_neighbors = 0;
        var current_state = 0;
        for(var row = 0; row < this.height; row++){
            for(var column = 0; column < this.width; column++){
                // 1 is life exists, 0 no life on the cell
                number_of_neighbors = this.get_neighbor_count(row,column);
                current_state = this.tiles[row][column];
                
                if(numberOfNeighbors < 2 || numberOfNeighbors > 3){
                    // Unable to live without neighbors, but not too many ;)
                    new_board.tiles[row][column] = 0;
        
                } else if (numberOfNeighbors == 3 && currentState == 0){
                    // any dead cell with exactly 3 live neighbours becomes a live cell
                    new_board.tiles[row][column] = 1;
                }               
            }
        }
        return new_board;
    },
    
    get_neighbor_count: function(row, column){
        int neighbors = 0;
        // subtract 1 if this is alive
        neighbors -= this.tiles[row][column];
        
        for(var row = -1; row < 2; row++){
            for(var column = -1; column < 2; column++){
                neighbors+= this.this[row][column];
            }
        }
        return neighbors;
    }
}