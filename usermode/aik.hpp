#pragma once

/*
mutex::lock
Aion values go into shared memory.

mutex::unlock

Gui reads shared memory and prints values.
Gui modifies shared memory value.

*/
class aik {
    aik();
    int read_client_values();
    int read_front_values();
};
