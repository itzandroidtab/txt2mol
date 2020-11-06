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

    long unsigned int file_len;
    uint8_t local_file[64] = {};

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

    // get the filename without any path or extention
    std::string filepath = argv[1];

    // compile if we are getting called with a txt file
    if (filepath.find(".TXT") != std::string::npos) {
        // create a copy of the filepath
        auto *c = strdup(filepath.c_str());

        // compile the file. This writes to the same folder with a .MOL extention
        (void) M05_compile_work_file(argv[1]);

        // free the char array memory
        free(c);
    }

    // search for the extention dot
    auto found = filepath.find_last_of(".");

    // check if we can strip the extention
    if (found != std::string::npos) {
        // strip the extention
        filepath = filepath.substr(0, found);
    }

    // add the .mol extention
    filepath += ".MOL";

    // check if we have a filename specified
    auto f = filepath.find_last_of("/\\");

    std::string filename = "";

    // check what we need to do with the filename and path
    if (f != std::string::npos) {
        // get the filename from the filepath
        filename = filepath.substr(f + 1);

        // remove the filename
        filepath = filepath.substr(0, f);
    }
    else {
        // set the filename as the filepath if we dont have any \\ or /
        filename = filepath;

        // clear the filepath
        filepath = "";
    }

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

    // create a copy of the filepath + filename (add a \ in between to fix the M05 bug with strange names)
    auto *c = strdup((filepath + "\\" + filename).c_str());

    // upload the file 
    M05_download_work_file(c);

    // free the memory
    free(c);

    // close the link
    M05_close_link(1);

    return 0;
}