gcc master.c -lrt -pthread -o master
gcc producer_named.c timing.c -o producer_named
gcc consumer_named.c timing.c -o consumer_named
gcc producer_unnamed.c timing.c -o producer_unnamed
gcc consumer_unnamed.c timing.c -o consumer_unnamed
gcc producer_socket.c timing.c -o producer_socket
gcc consumer_socket.c timing.c -o consumer_socket
gcc producer_shm.c timing.c -lrt -pthread -o producer_shm
gcc consumer_shm.c timing.c -lrt -pthread -o consumer_shm
./master