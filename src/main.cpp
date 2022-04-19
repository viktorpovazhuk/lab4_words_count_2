#include <iostream>
#include "thread_safe_queue.h"
#include <filesystem>
#include <string>
#include <fstream>
#include "files_methods.h"
#include "options_parser.h"
#include "errors.h"
#include "thread_functions.h"
#include "write_in_file.h"
#include "time_measurement.h"

namespace fs = std::filesystem;

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

//#define PRINT_CONTENT
#define PARALLEL

void findAndCountWords(int numberOfThreads, std::vector<std::thread> &threads, ThreadSafeQueue<string> &filesContents,
                       std::unordered_map<std::string, int> &wordsDict, std::mutex &globalDictMutex,
                       TimePoint &timeFinding);

int main(int argc, char *argv[]) {
    string configFilename;
    if (argc < 2) {
        configFilename = "index.cfg";
    } else {
        std::unique_ptr<command_line_options_t> command_line_options;
        try {
            command_line_options = std::make_unique<command_line_options_t>(argc, argv);
        }
        catch (std::exception &ex) {
            cerr << ex.what() << endl;
            return Errors::OPTIONS_PARSER;
        }
        configFilename = command_line_options->config_file;
    }

    std::unique_ptr<config_file_options_t> config_file_options;
    try {
        config_file_options = std::make_unique<config_file_options_t>(configFilename);
    }
    catch (OpenConfigFileException &ex) {
        cerr << ex.what() << endl;
        return Errors::OPEN_CFG_FILE;
    } catch (std::exception &ex) {
        cerr << ex.what() << endl;
        return Errors::READ_CFG_FILE;
    }

    std::string fn = config_file_options->out_by_n;
    std::string fa = config_file_options->out_by_a;
    int numberOfThreads = config_file_options->indexing_threads;

    FILE *file;
    file = fopen(fn.c_str(), "r");
    if (file) {
        fclose(file);
    } else {
        std::ofstream MyFile(fn);
        MyFile.close();
    }

    file = fopen(fa.c_str(), "r");
    if (file) {
        fclose(file);
    } else {
        std::ofstream MyFile(fa);
        MyFile.close();
    }

    auto timeStart = get_current_time_fenced();
    TimePoint timeFindingFinish, timeReadingFinish, timeWritingFinish;

    ThreadSafeQueue<fs::path> paths;
    ThreadSafeQueue<string> filesContents;
    filesContents.setMaxElements(100);
    std::unordered_map<std::string, int> wordsDict;

    std::mutex globalDictMutex;
    std::vector<std::thread> threads;
    threads.reserve(numberOfThreads);

#ifdef PARALLEL
    std::thread filesEnumThread(findFiles, std::ref(config_file_options->indir), std::ref(paths));

    std::thread filesReadThread(readFiles, std::ref(paths), std::ref(filesContents), std::ref(timeReadingFinish));

    findAndCountWords(numberOfThreads, threads, filesContents, wordsDict, globalDictMutex, timeFindingFinish);

    // TODO: 1. create shared var numOfWorkingIndexers
    //  2. start separate threads for indexing and merging
    //  3. add numOfWorkingIndexers decrement in indexing function

    if (filesEnumThread.joinable()) {
        filesEnumThread.join();
    }

    if (filesReadThread.joinable()) {
        filesReadThread.join();
    }

    try {
        for (auto &thread: threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    } catch (std::error_code e) {
        std::cerr << "Error code " << e << ". Occurred while joining threads." << std::endl;
    }

#else
    paths.setMaxElements(999999);
    filesContents.setMaxElements(999999);


    findFiles(config_file_options->indir, paths);

#ifdef PRINT_CONTENT
    auto path = paths.deque();
    paths.enque(path);
    while (path != fs::path("")) {
        cout << path << '\n';
        path = paths.deque();
        paths.enque(path);
    }
    cout << "--------------------------" << '\n';
#endif

    readFiles(paths, filesContents, timeReadingFinish);

#ifdef PRINT_CONTENT
    auto content = filesContents.deque();
    filesContents.enque(content);
    while (!content.empty()) {
        cout << content << '\n';
        content = filesContents.deque();
        filesContents.enque(content);
    }
    cout << "--------------------------" << '\n';
#endif

    overworkFile(filesContents, wordsDict, globalDictMutex, timeFindingFinish);
#endif

    auto timeWritingStart = get_current_time_fenced();

    writeInFiles(fn, fa, wordsDict);

    timeWritingFinish = get_current_time_fenced();

    auto totalTimeFinish = get_current_time_fenced();

    auto timeReading = to_us(timeReadingFinish - timeStart);
    auto timeFinding = to_us(timeFindingFinish - timeStart);
    auto timeWriting = to_us(timeWritingFinish - timeWritingStart);
    auto timeTotal = to_us(totalTimeFinish - timeStart);

    cout << "Total=" << timeTotal << "\n"
         << "Reading=" << timeReading << "\n"
         << "Finding=" << timeFinding << "\n"
         << "Writing=" << timeWriting;

    return 0;
}

void findAndCountWords(int numberOfThreads, std::vector<std::thread> &threads, ThreadSafeQueue<string> &filesContents,
                       std::unordered_map<std::string, int> &wordsDict, std::mutex &globalDictMutex,
                       std::chrono::time_point<std::chrono::high_resolution_clock> &timeFindingFinish) {
    try {
        for (int i = 0; i < numberOfThreads; i++) {
            threads.emplace_back(overworkFile, std::ref(filesContents), std::ref(wordsDict), std::ref(globalDictMutex),
                                 std::ref(timeFindingFinish));
        }
    } catch (std::error_code e) {
        std::cerr << "Error code " << e << ". Occurred while splitting in threads." << std::endl;
    }
}