# README File

# CS 480 Operating Systems
# Programming Assignment 2 / Experimental Shell "XSH"

## Team Members 
# Name: Jeremiah Dobes
# RedID: 826788151
# Edoras: cssc4429

# Name: Anthony Ngo
# RedID: 825252396
# Edoras: cssc4410
#	* Account to be used for testing in edoras

## File Structure

- `main.cpp`: Contains the main program logic
- `MakeFile`: Script for compiling the program
- `README.md`: This file, providing instructions and information about the program.


## Compile Instructions 
- Make sure that a GCC or G++ compiler is installed on your system
- Open up an terminal window and navigate your way to the "a2" directory containing the source files.
- Run the command "make" to compile the source code 'main.cpp'.

## Operating Instructions 
- Following compilation run the executable that is produced with the following command "./xsh".
- Type common commands such as "ls" and "ls -a" into the command line and check out the outputs.
- Test the pipe function for example "ls | sort"

## File Descriptions
main.cpp - is the core source file of the file that contains the implementation of the following functionalities. 
   - User input handling from the shell.
   - Parsing of user inputted commands and identifies the use of pipe | for command separation.
   - Includes a exit command in order to terminate the current shell that is open. 

## Known Issues 
During our development Jeremiah and I did not run into any particular issues other than tinkering with the pipe function.

## Design Decisions
We didn't stray away from the instructions to add anything fancy. With the example outputs we wanted to ensure we had something close to picture perfect. 

## Lessons Learned
Jeremiah and I learned the following:

- How to implement our own shell  
- Command line input handling
- Improved error handling skills
- Creating a Makefile with a structured layout
- Better understanding of the implementation of pipes

