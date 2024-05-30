# README File

# CS 480 Operating Systems
# Programming Assignment 1 / Chat Bot 
## Team Members 
# Name: Jeremiah Dobes
# RedID: 826788151
# Edoras: cssc4429

# Name: Anthony Ngo
# RedID: 825252396
# Edoras: cssc4410

## File Structure

- `main.cpp`: Contains the main program logic
- `MakeFile`: Script for compiling the program
- `README.md`: This file, providing instructions and information about the program.


## Compile Instructions 
- Make sure that a GCC or G++ compiler is installed on your system
- Open up an terminal window and navigate your way to the "a1" directory containing the source files.
- Run the command "make" to compile the source code ,main.cpp'.

## Operating Instructions 
- Following compilation run the executable that is produced with the following command "./bots".
- Check out the outputs using 'cat' <filename> Example "cat QUOTE.txt"

## File Descriptions
main.cpp - is the core source file of the file that contains the implementation of the following functionalities. 
    - creates a file named Quote.txt in the current working directory and displays the process ID of each running proccess
    - initializes a semaphore named FLAG which is utilized to manage the access to the file Quote.txt
    - Creates seven threads using the designated thread library (POSIX)
    - Ensures that the even and odd numbered threads sleep for 2 and 3 seconds
    - Cleans up the semaphore Flag by using sem_destroy and exiting gracefully.

## Known Issues 
During our development Jeremiah and I did not run into any particular issues other than simple printing layouts. 

## Design Decisions
We didn't stray away from the instructions to add anything fancy. With the example outputs we wanted to ensure we had something close to picture perfect. 

## Lessons Learned
Jeremiah and I learned the following:

- Setting up WSL 
- Seperating code to varying threads
- Creating a structured README file
- Creating a Makefile with a structured layout

