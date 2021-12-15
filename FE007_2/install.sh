if [ $# -eq 0 ]		#A <pathname> is required
then
	echo "Usage: ./install.sh <pathname> "; 
	exit;
fi

if [ ! -d $1 ] #if the directory doesn't exist make the desired directory
then
	echo "Directory $1 DOES NOT exist.";
	while true; do
		read -p "Do you wish to create $1 directory? [Y/n] " yn
		case $yn in
			[Y]* ) mkdir $1; break;;	#make the directory
			[n]* ) exit;;
			* ) "Please, press Y for yes or n for no.";;
		esac
	done
fi

echo "Begin program installation on $1 ... ";


unzip ./src.zip;	#Unzip the src folder
mv ./src $1;		#move the unzipped src folder in the <pathname> directiory
#cp ./help.sh $1;	#copy the help bash in the <pathname> directiory
cp run.sh $1;		#copy the run bash in the <pathname> directiory
#cp ./README.txt $1;	#copy the README.txt in the <pathname> directiory
#cp ./general_instructions.txt $1;	#copy the general_instructions.txt in the <pathname> directiory


echo "Program installed on $1";

dpkg --status konsole &> /dev/null 	#Checking if the konsole pkg already exist
if [ $? -eq 0 ]
then
	echo "konsole pkg already installed."
else										#If the pkg is not installed it will be installed
	echo "For compiling, konsole pkg is needed.";
	while true; do
		read -p "Do you wish to continue? [Y/n] " yn #installing the konsole pkg for running
		case $yn in
			[Y]* ) sudo apt-get install -y konsole; break;; # You need to be superuser!
			[n]* ) exit;;
			* ) "Please, answer Y for yes or n for no.";;
		esac
	done
fi

echo "Begin sources' compilation ...";

#Compiling all the scripts and moving the executables in the <pathname> directory.
#The <pathname> directory will contain the src folder, the help.sh and run.sh bash scripts and a README.txt
#In the "src" folder there's a directorry called "executables" in which all the executables will be moved gcc master.c -lrt -pthread -o master
gcc $1/src/master/master.c -lrt -pthread -o $1/src/executables/master;
gcc $1/src/producer_named/producer_named.c -o $1/src/executables/producer_named;
gcc $1/src/consumer_named/consumer_named.c -o $1/src/executables/consumer_named;
gcc $1/src/producer_unnamed/producer_unnamed.c -o $1/src/executables/producer_unnamed;
gcc $1/src/consumer_unnamed/consumer_unnamed.c -o $1/src/executables/consumer_unnamed;
gcc $1/src/producer_socket/producer_socket.c -o $1/src/executables/producer_socket;
gcc $1/src/consumer_socket/consumer_socket.c -o $1/src/executables/consumer_socket;
gcc $1/src/producer_shm/producer_shm.c -lrt -pthread -o $1/src/executables/producer_shm;
gcc $1/src/consumer_shm/consumer_shm.c -lrt -pthread -o $1/src/executables/consumer_shm;

echo "You can run the program in $1 with ./run.sh ";
