# Linux Command Shell





	The entered commands run in parallel


	BATCH MODE
	usage : ./Shell filename

	a) If an attempt is made to run something other than ./shell example.txt, such as ./shell example.txt example2.txt, at startup, the program will terminate with an error message such as too many startup arguments.
	b) If the given txt cannot be found or opened, the program will give an error message and terminate.
	c) If there are more than 512 lines of command (not counting blank lines) in the file, the program will terminate with an appropriate error message.
	d) If the "quit" command appears in the file or the reading reaches the end of the file, the program will terminate automatically.


	INTERACTIVE MODE
	usage : ./Shell

	a) Pressing ENTER on the empty command line does not print an error message and does not affect the program.
	b) If the "quit" command is entered, the program terminates without an error message.
	c) If 'Ctrl-D' keys are pressed, the running process is interrupted and the program is terminated.

	GENERAL

	a) If an attempt is made to enter an input longer than 512 characters on a line, the program continues to run by printing an appropriate error message.
	b) The number of commands that can be processed at a time, that is, the maximum number of commands that can be written on one line is 16 (excluding arguments). If this limit is exceeded, the program writes an appropriate error message and continues to run.
	c) A command can have a maximum of 63 arguments. If this limit is exceeded, the program writes an appropriate error message and continues to run.
	d) If one line starts with semicolon(';') or there is no command used between two semicolons(';') , this is a syntax error for our program and the program continues to run by printing a corresponding message.
	e) If the command to be run cannot be executed or does not exist, an appropriate error message is printed and the program continues to run.
	f) The extra spaces in the command line mean nothing to the program and the program will continue to run as long as there are no problems with the commands and arguments.
	g) If any memory allocation (malloc) error is encountered during the execution of the program, the program will terminate by printing an error message.
	h) If a child process cannot be created for the command to be run, the program indicates this with an error message, the command cannot be executed, but the program continues to run.
	i) The "cd" command has been made available in the program so that the directory can be changed. Also, the directory the user is in can be seen in the prompt text.
