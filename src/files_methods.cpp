//
// Created by vityha on 27.03.22.
//

#include "files_methods.h"

#include "ReadFile.h"

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

void findFiles(string &filesDirectory, ThreadSafeQueue<fs::path> &paths) {
    for (const auto &dir_entry: fs::recursive_directory_iterator(filesDirectory)) {
        if (dir_entry.path().extension() == ".zip" || dir_entry.path().extension() == ".txt") {
            paths.enque(dir_entry.path());
        }
    }
    paths.enque(fs::path(""));
}

void readFiles(ThreadSafeQueue<fs::path> &paths, ThreadSafeQueue<ReadFile> &filesContents, TimePoint &timeReadingFinish) {
    auto path = paths.deque();
    while (path != fs::path("")) {
        ReadFile readFile;
        if (path.extension() == ".txt") {
            std::ifstream ifs(path);
            string content{(std::istreambuf_iterator<char>(ifs)),
                           (std::istreambuf_iterator<char>())};
            readFile.content = std::move(content);
        }
        else if (path.extension() == ".zip") {
            std::ifstream raw_file(path, std::ios::binary);
            std::ostringstream buffer_ss;
            buffer_ss << raw_file.rdbuf();
            std::string content{buffer_ss.str()};
            readFile.content = std::move(content);
        }
        readFile.extension = path.extension();
        readFile.filename = path.filename();
        filesContents.enque(std::move(readFile));
        path = paths.deque();
    }
    ReadFile emptyReadFile;
    filesContents.enque(emptyReadFile);
    paths.enque(fs::path(""));

    timeReadingFinish = get_current_time_fenced();
}