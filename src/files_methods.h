//
// Created by vityha on 27.03.22.
//

#ifndef INDEX_FILES_FILES_METHODS_H
#define INDEX_FILES_FILES_METHODS_H

#include "thread_safe_queue.h"
#include "time_measurement.h"
#include "ReadFile.h"

#include <archive.h>
#include <archive_entry.h>
#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

using std::string;

void findFiles(string &filesDirectory, ThreadSafeQueue<fs::path> &paths);
void checkZip(ThreadSafeQueue<ReadFile> &filesContents, ReadFile *file);
void readFiles(ThreadSafeQueue<fs::path> &paths, ThreadSafeQueue<ReadFile> &filesContents, int maxFileSize, std::chrono::time_point<std::chrono::high_resolution_clock> &timeReadingFinish);

#endif //INDEX_FILES_FILES_METHODS_H
