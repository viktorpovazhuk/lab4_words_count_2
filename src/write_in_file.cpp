//
// Created by petro on 06.04.2022.
//

#include "write_in_file.h"



void writeInFiles(std::string& numResults, std::string& alphResults, std::unordered_map<std::string, int> &dict){

    std::ofstream myfile;
    try{
    myfile.open(numResults);
    std::set<std::pair<std::string, int>, cmpByNum> S(dict.begin(), dict.end());

    for(auto& el : S) {
        myfile << el.first << " = " << el.second << '\n';
    }
    myfile.close();
    } catch (std::error_code e){
        std::cerr << "Error code "<< e << ". Occurred while sorting dict by numbers and writing in file" << std::endl;
    }

    try{
    myfile.open(alphResults);
    std::set<std::pair<std::string, int>, cmpByAlph> S1(dict.begin(), dict.end());
    for(auto& el : S1) {
        myfile << el.first << " = " << el.second << '\n';
    }
    myfile.close();
    } catch (std::error_code e){
        std::cerr << "Error code "<< e << ". Occurred while sorting dict by alphabet and writing in file" << std::endl;
    }
}

