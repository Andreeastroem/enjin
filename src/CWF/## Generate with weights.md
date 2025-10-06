## Generate with weights

1. Fill grid with empty tiles with the same chance for neighbours in all directions
2. Find minimum entropy tile, choose a Tile
3. Propagate constraints
   1. empty tiles in each direction will have their potential states updated
      1. Possible states are remove
      2. Remaining possible states have their weights set based on neighbours
4. repeat
