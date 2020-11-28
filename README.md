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


