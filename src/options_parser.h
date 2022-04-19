#ifndef OPTIONS_PARSER_CONFIG_FILE_H
#define OPTIONS_PARSER_CONFIG_FILE_H

#include <boost/program_options.hpp>
#include <string>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>

using std::string;

class OptionsParseException : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class OpenConfigFileException : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class options_parser_t {
public:
    options_parser_t() = default;

    //! Explicit is better than implicit:
    options_parser_t(const options_parser_t &) = default;

    options_parser_t &operator=(const options_parser_t &) = delete;

    options_parser_t(options_parser_t &&) = default;

    options_parser_t &operator=(options_parser_t &&) = delete;

    ~options_parser_t() = default;

protected:
    boost::program_options::variables_map var_map{};
    boost::program_options::options_description general_opt{};
    boost::program_options::positional_options_description positional_opt{};
};

class command_line_options_t : public options_parser_t {
public:
    command_line_options_t();

    command_line_options_t(int ac, char **av);

    //! Explicit is better than implicit:
    command_line_options_t(const command_line_options_t &) = default;

    command_line_options_t &operator=(const command_line_options_t &) = delete;

    command_line_options_t(command_line_options_t &&) = default;

    command_line_options_t &operator=(command_line_options_t &&) = delete;

    ~command_line_options_t() = default;

    void parse(int ac, char **av);

    std::string config_file;
};

class config_file_options_t : public options_parser_t {
public:
    config_file_options_t();

    config_file_options_t(const std::string& config_file);

    void parse(const std::string& config_file);

    string indir, out_by_a, out_by_n;
    int indexing_threads = 0;
};

#endif //OPTIONS_PARSER_CONFIG_FILE_H

