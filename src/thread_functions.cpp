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

        if (file.extension == ".zip"){
            struct archive *a;
            struct archive_entry *entry;
            int r;
            int64_t length;
            void *buf;

            a = archive_read_new();
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);
            std::ifstream raw_file(file.content, std::ios::binary);
            raw_file.seekg(0, std::ios::end);
            size_t file_size = raw_file.tellg();

            r = archive_read_open_filename(a, file.content.c_str(), file_size);
            if (r != ARCHIVE_OK) {
                exit(26);
            }
            while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                ReadFile readFile;
                std::string name = archive_entry_pathname(entry), ext;
                int p = name.find('.');
                ext = name.substr(p, name.size());
                name = name.substr(0, p);

                if (ext == ".txt") {
                    length = archive_entry_size(entry);
                    buf = malloc(length);
                    archive_read_data(a, buf, length);

                    std::string content{(char *) buf};
                    readFile.content = std::move(content);
                    free(buf);
                    readFile.extension = ".txt";
                    readFile.filename = name;
                    filesContents.enque(std::move(readFile));
                }
            }
        } else {
            try {
                file.content = boost::locale::normalize(file.content);
                file.content = boost::locale::fold_case(file.content);

            } catch (std::error_code e) {
                printf("Indexing error");
            }

            boost::locale::boundary::ssegment_index words(boost::locale::boundary::word, file.content.begin(),
                                                          file.content.end());
            words.rule(boost::locale::boundary::word_letters);

            for (auto &word: words) {
                if (localDict.find(word) != localDict.end()) {
                    localDict.find(word)->second += 1;
                } else {
                    localDict.insert({word, 1});
                }
            }


            dicts.enque(localDict);

            localDict.clear();
        }





#ifdef SERIAL
            fileNumber++;
            std::cout << fileNumber << "\n";
#endif
        }
        if (numOfWorkingIndexers == 1) {
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