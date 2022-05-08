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

        printf("check - %s\n", file.content.c_str());
        boost::locale::boundary::ssegment_index words(boost::locale::boundary::word, file.content.begin(), file.content.end());
        words.rule(boost::locale::boundary::word_letters);

        try{
            boost::locale::normalize(file.content);
            boost::locale::fold_case(file.content);



        } catch(std::error_code e){
            printf("Indexing error");
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