gcc ./src/master/master.c -lrt -pthread -o ./src/executables/master;
gcc ./src/producer_named/producer_named.c -o ./src/executables/producer_named;
gcc ./src/consumer_named/consumer_named.c -o ./src/executables/consumer_named;
gcc ./src/producer_unnamed/producer_unnamed.c -o ./src/executables/producer_unnamed;
gcc ./src/consumer_unnamed/consumer_unnamed.c -o ./src/executables/consumer_unnamed;
gcc ./src/producer_socket/producer_socket.c -o ./src/executables/producer_socket;
gcc ./src/consumer_socket/consumer_socket.c -o ./src/executables/consumer_socket;
gcc ./src/producer_shm/producer_shm.c -lrt -pthread -o ./src/executables/producer_shm;
gcc ./src/consumer_shm/consumer_shm.c -lrt -pthread -o ./src/executables/consumer_shm;