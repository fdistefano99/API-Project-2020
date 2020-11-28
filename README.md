# Final examination of Algorithms and Data Structures
The examination consists in the creation of a line editor based on the known line editor 'Ed' for Unix operating systems.

The editor works through a command line interface and accepts the following commands:
* (add1, add2)c --->Changes the text lines between add1 and add2, after this command there must be add2-add1+1 text lines and at the end a point.Please note that 
the line editor can only add text lines "on the top"
* (add1, add2)p --->Prints the text lines between add1 and add2.
* (add1, add2)d --->Deletes the text lines between add1 and add2.
* (num)u --->Undo the last num commands.
* (num)r ---> Redo num commands

## Implementation


