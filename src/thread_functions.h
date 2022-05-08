//
// Created by petro on 27.03.2022.
//

#ifndef SERIAL_THREAD_FUNCTIONS_H
#define SERIAL_THREAD_FUNCTIONS_H

#include <iostream>
#include "thread_safe_queue.h"
#include <filesystem>
#include <string.h>
#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <locale>
#include <archive.h>
#include <archive_entry.h>
#include "time_measurement.h"
#include "ReadFile.h"
#include "boost/locale.hpp"


void overworkFile(ThreadSafeQueue<ReadFile> &filesContents, int &numOfWorkingIndexers, std::mutex& numOfWorkingIndexersMutex, std::chrono::time_point<std::chrono::system_clock> &timeIndexingFinish, ThreadSafeQueue<std::map<std::string, int>> &dicts);

void indexFile(std::vector <std::string> &words, std::string& file);

void mergeDicts(ThreadSafeQueue<std::map<std::string, int>> &dictsQueue, std::chrono::time_point<std::chrono::high_resolution_clock> &timeMergingFinish);

std::map<std::string, int> getDict(ThreadSafeQueue<std::map<std::string, int>> &dictsQueue, int &numOfWorkingIndexers);

#endif //SERIAL_THREAD_FUNCTIONS_H
