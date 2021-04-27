//
// Created by Robert Schmoltzi on 27.04.21.
//

#include "Util.h"
#include <string>
#include <sstream>
#include <algorithm>

std::string Util::getOutputOfBashCommand(const std::string &cmd) {
    std::string data;
    FILE *      stream;
    const int   max_buffer = 256;
    char        buffer[max_buffer];
    auto manipCommand = cmd;
    manipCommand.append(" 2>&1");

    stream = popen(manipCommand.c_str(), "r");

    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
    return data;
}

std::vector<std::string> Util::split(const std::string& str, const char delim) {
    std::vector<std::string> ret;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        ret.push_back(token);
    }
    return ret;
}
