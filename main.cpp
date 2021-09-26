#include <windows.h>
#include <iostream>
#include <algorithm>

#include <mpc65xx.hpp>

int main(int argc, char ** argv) {
    // check if we have a file as argument
    if (argc < 2) {
        std::cerr << "Not enough arguments. Usage:" << std::endl
                  << "text_to_mol file" << std::endl;

        return -1;
    }

    // create a mpc object
    mpc::mpc65xx mpc65xx(1, 115200);

    // try to open the connection
    if (!mpc65xx.open()) {
        std::cerr << "Something went wrong initilizing the board" << std::endl;

        // early return to prevent execution of code below
        return -1;
    }

    // get a string version of the argument
    std::string file_argv = std::string(argv[1]);

    // search for the extention dot
    auto found = file_argv.find_last_of(".");

    // get the extention of the file
    std::string extention;

    // check if we can strip the extention
    if (found != std::string::npos) {
        // strip the extention
        extention = file_argv.substr(0, found);
    }

    // compile if we are getting called with a txt file
    if (extention.find(".TXT") != std::string::npos || 
        extention.find(".txt") != std::string::npos) {
        std::cout << "Compiling file." << std::endl;

        // create a copy of the argument
        char *file_path_buffer = strdup(file_argv.c_str());

        // compile the file. This writes to the same folder with a .MOL extention
        (void) M05_compile_work_file(file_path_buffer);

        // free the char array memory
        free(file_path_buffer);

        // change the extention to a .mol extention
        extention += ".MOL";        
    }

    // split the filepath, filename and extention.
    std::string path;
    std::string filename;

    // check if we have a filename specified
    auto path_result = file_argv.find_last_of("/\\");

    // check what we need to do with the filename and path
    if (path_result != std::string::npos) {
        // get the path
        path = file_argv.substr(0, path_result);

        // get the filename
        filename = file_argv.substr(path_result + 1);
    }
    else {
        // clear the filepath
        path = "";

        // set the filename as the filepath if we dont have any \ or /
        filename = file_argv;        
    }

    // change the filename to upper
    for (auto &c: filename) {
        c = toupper(c);
    }    

    // get al the files
    auto files = mpc65xx.get_files();

    // check if the file already exists on the machine
    auto find_result = std::find_if(files.begin(), files.end(), [&filename](mpc::file_s file) {
        // return if the file name is the same
        return filename == file.file;
    });

    // delete the file if it exists
    if (find_result != files.end()) {
        // delete the file
        mpc65xx.delete_file((*find_result).id);
    }

    std::cout << "Uploading file." << std::endl;

    // upload the filepath + filename (add a \ in between to fix the M05 bug with strange names)
    mpc65xx.upload_file(path + "\\" + filename);

    std::cout << "Done uploading file." << std::endl;

    // close the link
    mpc65xx.close();

    return 0;
}