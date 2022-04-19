//
// Created by petro on 27.03.2022.
//

#ifndef SERIAL_THREAD_FUNCTIONS_H
#define SERIAL_THREAD_FUNCTIONS_H

#include <iostream>
#include "thread_safe_queue.h"
#include <filesystem>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <locale>
#include "time_measurement.h"


void overworkFile(ThreadSafeQueue<std::string> &filesContents, std::unordered_map<std::string, int>& dict, std::mutex &mut, std::chrono::time_point<std::chrono::high_resolution_clock> &timeFindingFinish);

void indexFile(std::vector <std::string> &words, std::string& file);

void mergeDicts(std::unordered_map<std::string, int> &dict, std::map<std::string, int> &localDict);

#endif //SERIAL_THREAD_FUNCTIONS_H
