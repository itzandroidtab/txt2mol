#ifndef MPC65XX_HPP
#define MPC65XX_HPP

#include <vector>
#include <cstdint>
#include <string>

#include <mpc05ls.hpp>

namespace mpc {
    struct file_s {
        // name of the file
        std::string file;

        // length of the file
        uint64_t length;

        // id of the file
        uint32_t id;
    };

    class mpc65xx {
        protected:
            // channel to the mpc65xx motion card
            const int channel;

            // rate of the mpc65xx motion card 
            const int rate;

        public:
            mpc65xx(const int channel = 1, const int rate = 115200): 
                channel(channel), rate(rate)
            {}

            /**
             * @brief Open the communication to the mpc65xx motion card
             * 
             * @return true 
             * @return false 
             */
            bool open() {
                // set the communication
                M05_set_comm(nullptr);

                // try to open the link to the laser machine
                M05_open_link(channel, rate);

                // init and check if result is true (init went wrong)
                if (M05_init_board()) {
                    // close the connection
                    M05_close_link(channel);

                    // init went wrong return
                    return false;
                }

                return true;
            }

            /**
             * @brief Close the communication to the mpc65xx motion card
             * 
             */
            void close() {
                // close the link
                M05_close_link(channel);            
            }

            /**
             * @brief Get the amount of files on the mpc65xx motion card
             * 
             * @return uint32_t 
             */
            uint32_t get_file_amount() {
                // create the variables for the m05 func
                int file_id, amount;

                // get the amount of files on the machine
                M05_get_file_num(&file_id, &amount);

                // return a minimum of 0
                return std::max(amount, 0);
            }

            /**
             * @brief Get all the files on the mpc65xx motion card
             * 
             * @return std::vector<std::string> 
             */
            std::vector<file_s> get_files() {
                // get the amount of files
                uint32_t amount = get_file_amount();

                // check if we have files on the machine
                if (!amount) {
                    // we have no files return
                    return {};
                }

                // return value
                std::vector<file_s> ret;

                // iterate over all the files
                for (uint32_t index = 1; index <= amount; index++) {
                    // lenght for the file
                    long unsigned int len = 0;

                    // pointer to a char array for the file name
                    uint8_t file_name[128] = {};

                    // open the file
                    M05_open_file(index, &len, file_name);

                    // create a file struct
                    file_s file = {
                        std::string(reinterpret_cast<const char*>(file_name)),
                        len, index
                    };

                    // add the file to the vector
                    ret.push_back(file);
                }

                // return the vector with the files
                return ret;
            }

            /**
             * @brief Delete a file with id
             * 
             * @param id 
             */
            void delete_file(int id) {
                // check if we have a valid id
                if (id > get_file_amount()) {
                    // id not valid return
                    return;
                }

                // delete the file
                M05_del_file(id);                
            }

            /**
             * @brief Upload a local file to the mpu65xx motion card
             * 
             * @param file 
             */
            void upload_file(std::string file) {
                // create a copy of the file for the m05 func
                auto *c = strdup(file.c_str());

                // upload the file 
                M05_download_work_file(c);

                // free the memory
                free(c);
            }
    };
}

#endif