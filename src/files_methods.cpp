//
// Created by vityha on 27.03.22.
//

#include "files_methods.h"

void findFiles(string &filesDirectory, ThreadSafeQueue<fs::path> &paths) {
    for (const auto &dir_entry: fs::recursive_directory_iterator(filesDirectory)) {
        if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".txt") {
            paths.enque(dir_entry.path());
        }
    }
    paths.enque(fs::path(""));
}

void readFiles(ThreadSafeQueue<fs::path> &paths, ThreadSafeQueue<string> &filesContents, std::chrono::time_point<std::chrono::high_resolution_clock> &timeReadingFinish) {
    auto path = paths.deque();
    while (path != fs::path("")) {
        std::ifstream ifs(path);
        string content((std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
        filesContents.enque(std::move(content));
        path = paths.deque();
    }
    filesContents.enque("");
    paths.enque(fs::path(""));

    timeReadingFinish = get_current_time_fenced();
}