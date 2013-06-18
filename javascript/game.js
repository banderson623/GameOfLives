// Game of Life in Javascript
// Brian Anderson


var Game = {
    // namespaced.
    Board: {},
    DOM: {},
    current: null,
    
    // Generates the board
    generate: function(height, width, seed){
        var new_board = Game.Board;
        new_board.height = height;
        new_board.width = width;
        var tiles = [];
        // todo, find out how to seed this
        for(var row = 0; row < height; row++){
            var a_row = [];
            for(var column = 0; column < width; column++){
                // 1 is life exists, 0 no life on the cell
                a_row[column] = Math.ceil(Math.random()*10) % 2
                console.log[a_row];
            }
            tiles[row] = a_row;
        }
        // copy, not reference, right?
        new_board.tiles = tiles;
        return new_board;
    },
    
    // set things up
    setup: function(height,width){
        this.current = this.generate(height,width);
    },
    
    
    step: function(){
        this.current = this.current.evolve();
    }
    
}
Game.new = function (height,width) {
    console.log("new was called on game");
    g = Game.clone()
};

Game.Board = {
    // tiles height, integer
    height: 1,
    // tile width, integer
    width: 1,
    tiles: [], // ends up being a 2d array.
    
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
        
                } else if (numberOfNeighbors == 3 && current_state == 0){
                    // any dead cell with exactly 3 live neighbors becomes a live cell
                    new_board.tiles[row][column] = 1;
                }               
            }
        }
        return new_board;
    },
    
    get_neighbor_count: function(row, column){
        var neighbors = 0;
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



Game.DOM = {
    container_element_name: "",
    container_element: null,
    
    dom_id_for_element: function(row,column){
        return "cell_" + row + "x" + column;
    },
    
    update_board_to_match:function (game){
        
    },
    
    build_board_in_element_identified_by_id: function(game,element_id){
        // this.container_element_name = element_id;
        this.container_element = document.getElementById(element_id);
        if(this.container_element !== null && this.current !== null){

            // Using createElement for the first time
            var table = document.createElement("table");
            table.id = "GameTable";
            
            for(row = 0; row < game.current.height; row++){
                var tr = document.createElement("tr");
                tr.id = "row_" + row;
                for(column = 0; column < game.current.width; column++){
                    var td = document.createElement("td");
                    td.id = this.dom_id_for_element(row,column);
                    // class is either life_0 or life_1;
                    td.className = "life_" + game.current.tiles[row][column];
                    var content = document.createTextNode(game.current.tiles[row][column]);
                    td.appendChild(content);
                    tr.appendChild(td);
                }
                table.appendChild(tr);
            }
        }
        // console.log("Inserting: ", bigTextBlob);
        // Game.DOM.container_element.innerHTML = bigTextBlob;
        Game.DOM.container_element.appendChild(table);
    }   
}