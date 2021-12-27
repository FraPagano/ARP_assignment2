Second Assignment of ARP class
---------------------------------------------------------------------

This project aims to compare different methods of IPC (InterProcess Communication) with various amounts of data to send.
It computes at each test performed the data transmission time in milliseconds and stores results in the test folder into the test_performed.txt file.
You can compare four different methods of communication between processes:

> Named pipes
> Unnamed pipes
> Socket
> Shared memory

In this project, there are nine different processes, the master one and two for each method of communication: producer and consumer. 
They are:

> master
> producer_named
> producer_unnamed
> producer_socket
> producer_shm
> consumer_named
> consumer_unnamed
> consumer_socket
> consumer_shm

The user must specify the number of MB transmitted; this is a float number less than or equal to one hundred.
All processes report the performed operations in a log file in the 'log_file' folder.
In the file parameters.h in the parameters folder are stored names, paths, and constants used in the executions.
The library timing.c in the timing folder defines some widely used functions: in particular, there are the functions to send the starting and the ending time to compute the duration of the communication, and the one to graphically represent a loading bar updated during the execution of the programs.


#### INSTALLATION ####

You can install the program with 'install.sh' bash script. For doing this, insert on the terminal:

$ ./install.sh <pathname>

Where <pathname> is the destination directory where all files will be copied. If it doesn't exist, the script will create it after asking the user, and the installation procedure will start.
Furthermore, it will compile all programs, placing the executables into the "executables" directory.


#### RUN ####

You can run the program with:

$ ./run.sh


#### HELP ####

If you need more information about each process, you can ask for help with:

$ ./help.sh
