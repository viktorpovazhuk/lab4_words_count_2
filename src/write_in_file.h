//
// Created by petro on 06.04.2022.
//

#ifndef SERIAL_WRITE_IN_FILE_H
#define SERIAL_WRITE_IN_FILE_H

#include <bits/stdc++.h>
#include "thread_safe_queue.h"
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <locale>

void writeInFiles(std::string& numResults, std::string& alphResults, std::unordered_map<std::string, int> &dict);


struct cmpByNum{

    bool operator()(const std::pair<std::string, int> &lhs, const std::pair<std::string, int> &rhs) const
    {
        if (lhs.second == rhs.second) {
            if (lhs.first[0] != rhs.first[0]){
                return lhs.first[0] > rhs.first[0];
            } else{
                int i = 0;
                while (lhs.first[i] == rhs.first[i]){
                    i++;
                    if (i == (int)rhs.first.length()){
                        return true;
                    } else if (i == (int)lhs.first.length()){
                        return false;
                    }
                }
                return lhs.first[i] > rhs.first[i];
            }
        }
        return lhs.second > rhs.second;
    }
};


struct cmpByAlph{

    bool operator()(const std::pair<std::string, int> &lhs, const std::pair<std::string, int> &rhs) const
    {
        if (lhs.first[0] != rhs.first[0]){
            return lhs.first[0] < rhs.first[0];
        } else{
            int i = 0;
            while (lhs.first[i] == rhs.first[i]){
                i++;
                if (i == (int)rhs.first.length()){
                    return true;
                } else if (i == (int)lhs.first.length()){
                    return false;
                }
            }
            return lhs.first[i] < rhs.first[i];
        }
    }
};

#endif //SERIAL_WRITE_IN_FILE_H
