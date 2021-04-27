//
// Created by Robert Schmoltzi on 27.04.21.
//

#ifndef CONCURRENT_GROWTABLE_UTIL_H
#define CONCURRENT_GROWTABLE_UTIL_H


#include <string>
#include <vector>

class Util {
public:
    /* Takes a string and executes it as a bash command. The output is returned as string.*/
    static std::string getOutputOfBashCommand(const std::string& cmd);
    /* Definitely should not be part of any standard library */
    static std::vector<std::string> split(const std::string& str, const char delim);
};


#endif //CONCURRENT_GROWTABLE_UTIL_H
