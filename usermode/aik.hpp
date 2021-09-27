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
    void init_shared_memory();
    int read_client_values_into_shared();
    void process_shared_memory();
};
