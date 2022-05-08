//
// Created by vityha on 19.04.22.
//

#ifndef COUNTER_READFILE_H
#define COUNTER_READFILE_H

#include <string>

struct ReadFile {
    void * buff;
    int64_t length;
    std::string content;
    std::string extension;
    std::string filename;
};

#endif //COUNTER_READFILE_H
