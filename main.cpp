#include <windows.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>

#include <mpc05ls.hpp>

int file_exists(std::string filename) {
    int file_id, file_num;

    // get the amount of files on the machine
    M05_get_file_num(&file_id, &file_num);

    // check if there are files on the machine
    if (file_num < 0) {
        return -1;
    }

    // search for the extention dot
    auto found = filename.find_last_of(".");

    // check if we can strip the extention
    if (found != std::string::npos) {
        // strip the extention
        filename = filename.substr(0, found);
    }

    // add the .mol extention
    filename += ".MOL";

    long unsigned int file_len;
    uint8_t local_file[16] = {};

    // check if a file with the name already exists
    for (size_t i = 0; i < file_id; i++) {
        // open the file
        M05_open_file(i + 1, &file_len, local_file);

        // convert the local filename to a string
        auto l = std::string(reinterpret_cast<char*>(local_file));

        // check if the local filename matches the name we are looking for
        if (l == filename) {
            // found the id we are looking for
            return i + 1;
        }
    }

    return -1;
}

int main(int argc, char ** argv) {
    // set the communication
    M05_set_comm(0);

    // try to open the link to the laser machine
    M05_open_link(1, 115200);

    // init the laser board/library
    if (M05_init_board()) {
        std::cout << "Something went wrong initilizing the board" << std::endl;

        // close the connection
        M05_close_link(1);

        return -1;
    }

    // compile the file
    int a = M05_compile_work_file(argv[1]);

    // get the filename without any path or extention
    std::string filepath = argv[1];
    // remove everything before the /
    std::size_t found = filepath.find_last_of("/\\");

    // get the filename from the filepath
    std::string filename = (found != std::string::npos) ? filepath.substr(found + 1) : filepath;

    // change the filename to upper
    for (auto & c: filename) {
        c = toupper(c);
    }

    // check if the file already exists
    auto r = file_exists(filename);

    // delete the file if it exists
    if (r >= 0) {
        // delete the file
        M05_del_file(r);
    }

    // upload the file
    M05_download_work_file(argv[1]);

    // close the link
    M05_close_link(1);

    return 0;
}