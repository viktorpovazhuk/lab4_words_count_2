//
// Created by petro on 27.03.2022.
//

#include "thread_functions.h"

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
using mapStrInt = std::map<std::string, int>;

//#define SERIAL

/*
overworkFile()

Main func in thread. Take element from the queue, index it and merge with global dict.

indexFile()

Split in words, format and count. Do it in reference of dict.

mergeDicts()

Merge to global dict.*/

void overworkFile(ThreadSafeQueue<ReadFile> &filesContents, int &numOfWorkingIndexers, std::mutex& numOfWorkingIndexersMutex, TimePoint &timeIndexingFinish, ThreadSafeQueue<std::map<std::string, int>> &dicts) {

    //std::chrono::time_point<std::chrono::high_resolution_clock> &timeFindingFinish

    std::map<std::string, int> localDict;
#ifdef SERIAL
    int fileNumber = 0;
#endif

    while (true) {
        ReadFile file;
        try {
            file = filesContents.deque();
            if (file.content == "") {
                // don't need mutex because queue is empty => other threads wait
                filesContents.enque(file);
                timeIndexingFinish = get_current_time_fenced();
                break;
            }
        } catch (std::error_code e) {
            std::cerr << "Error code " << e << ". Occurred while working with queue in thread." << std::endl;
            continue;
        }

/*        if (file.extension == ".zip"){
            printf("zip founded\n");
            struct archive *a;
            struct archive_entry *entry;
            int r;
            int64_t length;
            void *buf;
            a = archive_read_new();
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);

            r = archive_read_open_memory(a, file.buff, file.length);
            if (r != ARCHIVE_OK){
                exit(23);
            }
            ReadFile readFile;
            while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                std::string name = archive_entry_pathname(entry), ext;
                int p = name.find('.');
                ext = name.substr(p, name.size());
                name = name.substr(0, p);
                if (ext == ".txt") {
                    length = archive_entry_size(entry);
                    buf = malloc(length);
                    archive_read_data(a, buf, length);
                    printf("%s\n", (char *) buf);

                    std::string content{(char *)buf};
                    readFile.content = std::move(content);
                    free(buf);
                    readFile.extension = ".txt";
                    readFile.filename = name;

                    filesContents.enque(std::move(readFile));
                } else if (name == ".zip" ) {
                    length = archive_entry_size(entry);
                    buf = malloc(length);
                    archive_read_data(a, buf, length);
                    printf("h - %s\n", (char *) buf);

                    readFile.buff = buf;
                    free(buf);
                    readFile.content = "not empty";
                    readFile.length = length;
                    readFile.extension = ".zip";
                    readFile.filename = name;
                    filesContents.enque(std::move(readFile));
                }
            }
            continue;

        }*/

        printf("check\n");
        std::vector<std::string> words;
        try{

            indexFile(words, file.content);
        } catch(std::error_code e){
            printf("Indexing error");
            exit(259);
        }


        for (auto &word: words) {
            if (localDict.find(word) != localDict.end()) {
                localDict.find(word)->second += 1;
            } else {
                localDict.insert({word, 1});
            }
        }


        dicts.enque(localDict);

        localDict.clear();

#ifdef SERIAL
        fileNumber++;
        std::cout << fileNumber << "\n";
#endif
    }
    if (numOfWorkingIndexers == 1){
        numOfWorkingIndexersMutex.lock();
        numOfWorkingIndexers = 0;
        numOfWorkingIndexersMutex.unlock();

        localDict.clear();
        dicts.enque(localDict);
    } else {
        numOfWorkingIndexersMutex.lock();
        numOfWorkingIndexers--;
        numOfWorkingIndexersMutex.unlock();

    }
}

void indexFile(std::vector<std::string> &words, std::string &file) {

    try {
        boost::locale::normalize(file);
        boost::locale::fold_case(file);
    } catch (std::error_code e) {
        std::cerr << "Error code " << e << ". Occurred while transforming word to lowercase" << std::endl;
    }


    size_t start_pos = 0;
    try {
        while ((start_pos = file.find(std::string("\n"), start_pos)) != std::string::npos) {
            file.replace(start_pos, std::string("\n").length(), std::string(" "));
            start_pos += std::string(" ").length();
        }

        start_pos = 0;
        while ((start_pos = file.find(std::string("\r"), start_pos)) != std::string::npos) {
            file.replace(start_pos, std::string("\r").length(), std::string(" "));
            start_pos += std::string(" ").length();
        }
    } catch (std::error_code e) {
        std::cerr << "Error code " << e << ". Occurred while deleting /n and /r from files" << std::endl;
    }


    try {
        std::stringstream s(file);
        std::string s2;

        while (std::getline(s, s2, ' ')) {
            words.push_back(s2);
        }
    } catch (std::error_code e) {
        std::cerr << "Error code " << e << ". Occurred while splitting file into words" << std::endl;
    }

}

void mergeDicts(ThreadSafeQueue<mapStrInt> &dictsQueue, TimePoint &timeMergingFinish) {
    while (true) {
        mapStrInt dict1 = dictsQueue.deque();
        while (dict1.empty()) {
            if (dictsQueue.empty()) {
                dictsQueue.enque(dict1);
                timeMergingFinish = get_current_time_fenced();
                return;
            } else {
                dictsQueue.enque(dict1);
                dict1 = dictsQueue.deque();
            }
        }
        mapStrInt dict2 = dictsQueue.deque();
        while (dict2.empty()) {
            if (dictsQueue.empty()) {
                dictsQueue.enque(dict1);
                dictsQueue.enque(dict2);
                timeMergingFinish = get_current_time_fenced();
                return;
            } else {
                dictsQueue.enque(dict2);
                dict2 = dictsQueue.deque();
            }
        }

        try {
            for (auto &i: dict1) {
                if (dict2.find(i.first) != dict2.end()) {
                    dict2.at(i.first) += i.second;
                } else {
                    dict2.insert({i.first, i.second});
                }
            }
        } catch (std::error_code &e) {
            std::cerr << "Error code " << e << ". Occurred while merging dicts" << std::endl;
        }

        dictsQueue.enque(std::move(dict2));
    }
}