You can compile like this: make
You can then run like this: make start

after you do CTRL + C, you might need to run make several times again to kill old processes listening to ports
make kills old processes listening on key ports when it runs

./tests - 2 test runs
./bin - binaries. Don't run them directly though, use make start instead

starter starts the whole thing. It execs philosopheres. Then on of them becomes coordinator
soc.h has common functions
queue.c is used only by coordinator

how do we know this works? 
Because the output has many lines like these: [Fri Nov 18 03:21:50 2022] COORDINAT:CHOPSTICKS: 0 1 0 0 0
It means that 2 philosophers are eating at the same time. After this line, you will typically either see that all of the chopsticks will be used, which means that a new philosopher requested one chopstick and will give it back, 
OR
three chopsticks will be present, meaning that one philosopher gave back 2 chopsticks

none of the philosophers will starve because a FIFO queue is used

deadlock will not happen because philosophers grab chosticks one at a time