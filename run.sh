gcc master.c -lrt -o master
gcc consumer_named.c -o consumer_named
gcc producer_named.c -o producer_named
gcc consumer_unnamed.c -o consumer_unnamed
gcc producer_unnamed.c -o producer_unnamed
gcc producer_socket.c -o producer_socket
gcc consumer_socket.c -o consumer_socket
gcc producer_shm.c -lrt -pthread -o producer_shm
gcc consumer_shm.c -lrt -pthread -o consumer_shm
./master
