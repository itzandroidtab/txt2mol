#include <windows.h>
#include <iostream>
#include <cstdint>

#include <mpc05ls.hpp>

int main(int argc, char ** argv) {
    M05_set_comm(0);

    M05_open_link(1, 115200);

    if (M05_init_board()) {
        std::cout << "Something went wrong initilizing the board" << std::endl;

        M05_close_link(1);

        return -1;
    }

    // compile the file
    int a = M05_compile_work_file(argv[1]);

    return 0;
}