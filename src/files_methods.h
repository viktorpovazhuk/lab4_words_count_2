//
// Created by vityha on 27.03.22.
//

#ifndef INDEX_FILES_FILES_METHODS_H
#define INDEX_FILES_FILES_METHODS_H

#include "thread_safe_queue.h"
#include "time_measurement.h"
#include <filesystem>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

using std::string;

void findFiles(string &filesDirectory, ThreadSafeQueue<fs::path> &paths);
void readFiles(ThreadSafeQueue<fs::path> &paths, ThreadSafeQueue<string> &filesContents, std::chrono::time_point<std::chrono::high_resolution_clock> &timeReadingFinish);

#endif //INDEX_FILES_FILES_METHODS_H
