#include <windows.h>
#include <iostream>

#include <mpc05ls.hpp>

int main(int argc, char ** argv) {
    using init_board_func = bool (*)();
    using compile_file_func = void (*)(const char *);

    if (argc < 2) {
        std::cout << "Please provide a file to process." << std::endl; 

        return -1;
    }

    // try to load the library
    auto mpc05_lib = LoadLibrary("mpc05ls.dll");

    // check if the load succeeded.
    if (!mpc05_lib) {
        std::cout << "Could not open library. Check for CommM05.dll and mpc05ls.dll" << std::endl;

        return -2;
    }

    // get the address of the mpc05 init board function in the library
    auto init_func = reinterpret_cast<init_board_func>(GetProcAddress(mpc05_lib, "M05_init_board"));

    if (!init_func) {
        std::cout << "Could not load M05_init_board function from dll" << std::endl;

        return -3;
    }

    // get the address of the compile function in the library
    auto compile_func = reinterpret_cast<compile_file_func>(GetProcAddress(mpc05_lib, "M05_compile_work_file"));

    if (!compile_func) {
        std::cout << "Could not load M05_compile_work_file function from dll" << std::endl;

        return -4;
    }

    if (init_func()) {
        std::cout << "Something went wrong initilizing the board" << std::endl;

        return -5;
    }

    // compile the file
    compile_func(argv[1]);

    return 0;
}