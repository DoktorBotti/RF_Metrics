//
// Created by Robert Schmoltzi on 27.04.21.
//

#ifndef CONCURRENT_GROWTABLE_UTIL_H
#define CONCURRENT_GROWTABLE_UTIL_H


#include <string>
#include <vector>
#include <PllSplits.hpp>

class Util {
public:
    /* Takes a string and executes it as a bash command. The output is returned as string.*/
    static std::string get_output_of_bash_command(const std::string& cmd);
    /* Definitely should not be part of any standard library */
    static std::vector<std::string> split(const std::string& str, char delim);
    static std::vector<PllSplitList> create_splits(int n, std::string file_name);
};


#endif //CONCURRENT_GROWTABLE_UTIL_H