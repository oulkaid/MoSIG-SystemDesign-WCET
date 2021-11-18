# User Manual
<-- Author: Oussama Oulkaid -->

## Level 0: Manual assembly code parser and CFG builder
This level of analysis is done in the `DoorControl_manual.py` file.

We are only filling a few elements of the control-flow graph (first 7 basic blocks). The goal here is to confirm the algorithm that we are going to implement later.

The program prints the elements of the data structure corresponding to each basic block.

You can simply run `python.exe DoorControl_manual.py` to have a look at the output.

## Levels 1 & 2: Assembly code parser & Graph reconstruction
Our code parser is the `DoorControl_cfg.c` file.

The file contain Three parts
1. parser of the assembly code file, and creation of the nodes representing the basic block. And build of the `.cfg` file representing the `ControleurPorte_step` function. The resulting CFG is in `output.cfg`.
2. CFG reconstruction, so that the data structure instances are linked according to the CFG dependencies.
3. Construction of  an additional `.csv` file as a proposition to facilitate plotting as a graph.

## [ Additional ] Sample plotting of a portion of the CFG
As explained within the `DoorControl_cfg.c`, the `summerize.csv` file can be used as a minimal representation of the CFG.

A basic trial have been made to plot a portion of the CFG (first 7 basic blocks). (performed by the `display_cfg.py` program)
