> Detailed description of the project is available in [`notebook.pdf`](notebook.pdf)

# Reconstruction of the Control Flow Graph (CDG) from assembly code
The goal of this lab is to reproduce the CFG of the main program in [`src/DoorControl.ass`](src/DoorControl.ass). For this purpose, we have split the analysis into several levels.

## Level 0: Manual assembly code parser and CFG builder
This level of analysis is done in the [`src/DoorControl_manual.py`](src/DoorControl_manual.py) file.

We are only filling a few elements of the control-flow graph (first 7 basic blocks). The goal here is to confirm the algorithm that we are going to implement later.

The program prints the elements of the data structure corresponding to each basic block.

You can run `python.exe DoorControl_manual.py` to have a look at the output.

## Levels 1 & 2: Assembly code parser & Graph reconstruction
The parser file is [`src/DoorControl_cfg.c`](src/DoorControl_cfg.c)

The file is composed of three parts:
1. Parser of the assembly code, and creation of the nodes representing the basic blocks. And build of the `output.cfg` file representing the `ControleurPorte_step` function. The resulting CFG is [`src/output.cfg`](src/output.cfg)
2. CFG reconstruction, so that the data structure instances are linked according to the CFG dependencies
3. Construction of an additional `.csv` file as a means to facilitate data visualization

## [ Additional ] Sample plotting of a portion of the CFG
As explained within the [`src/DoorControl_cfg.c`](src/DoorControl_cfg.c), the [`src/summarize.csv`](src/summarize.csv) file can be viewed as a minimal representation of the CFG

An initial trial have been made to plot a portion of the CFG (first 7 basic blocks). (performed by the [`src/display_cfg.py`](src/display_cfg.py) script)
