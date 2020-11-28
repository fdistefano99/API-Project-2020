# Final examination of Algorithms and Data Structures
The examination consists in the implementation in C language of a line editor based on the known line editor 'Ed' for Unix operating systems.

The editor works through a command line interface and accepts the following commands:
* (add1, add2)c ---> Changes the text lines between add1 and add2, after this command there must be add2-add1+1 text lines and a last line formed by a single point. Please note that the line editor can only add text lines starting from the first empty line, that is, it cannot leave "holes"
* (add1, add2)p ---> Prints the text lines between add1 and add2
* (add1, add2)d ---> Deletes the text lines between add1 and add2
* (num)u ---> Undo the last num commands
* (num)r ---> Redo num commands
* q ---> Exits the program

## Implementation
The text lines and the command history are implemented both in the same way.

There are 2 vectors(called super-vectors) whose size is set by the parameter HASHDIM and every cell of these vectors contains a vector of strings, for the super-vector of the text lines, or a vector of commands, for the command history one. 

At the beginning of the program only the first cells of the super-vectors contain vectors and the size of these one are setted by the parameter INITDIM. 

If the program needs more space for the text lines or the command history, the second cell of a super-vector is used for create a new vector of text lines or commands that is twice the size of the vector in the first cell of the super-vector and so on.

## Evaluation
The project has passed every test case and it has been evaluated with a grade of 30/30 cum laude.


