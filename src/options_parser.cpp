// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "options_parser.h"

namespace po = boost::program_options;

using std::string;

command_line_options_t::command_line_options_t() {
    general_opt.add_options()
            ("help,h",
             "Show help message")
            ("config_file",
             po::value<string>(),
             "File with configuration");

    positional_opt.add("config_file", 1);
}

command_line_options_t::command_line_options_t(int ac, char **av) :
        command_line_options_t() // Delegate constructor
{
    parse(ac, av);
}

void command_line_options_t::parse(int ac, char **av) {
    try {
        po::parsed_options parsed = po::command_line_parser(ac, av)
                .options(general_opt)
                .positional(positional_opt)
                .run();
        po::store(parsed, var_map);
        notify(var_map);

        if (var_map.count("help")) {
            std::cout << general_opt << "\n";
            exit(EXIT_SUCCESS);
        }

        config_file = var_map["config_file"].as<string>();
    } catch (std::exception &ex) {
        throw OptionsParseException(ex.what()); // Convert to our error type
    }
}

config_file_options_t::config_file_options_t() {
    general_opt.add_options()
            ("indir", po::value<string>(), "Input directory")
            ("out_by_a", po::value<string>(), "Path of alphabetically sorted result file")
            ("out_by_n", po::value<string>(), "Path of numerically sorted result file")
            ("indexing_threads", po::value<int>(), "Number of threads");
}

config_file_options_t::config_file_options_t(const string& config_file) :
        config_file_options_t() // Delegate constructor
{
    parse(config_file);
}

void config_file_options_t::parse(const string& config_file) {
    try {
        std::ifstream ifs{config_file.c_str()};
        if (ifs) {
            po::store(parse_config_file(ifs, general_opt), var_map);
        }
        else {
            throw OpenConfigFileException("Can't open config file");
        }
        notify(var_map);

        indir = var_map["indir"].as<string>();
        indir.erase(std::remove(indir.begin(), indir.end(), '\"'), indir.end());
        out_by_a = var_map["out_by_a"].as<string>();
        out_by_a.erase(std::remove(out_by_a.begin(), out_by_a.end(), '\"'), out_by_a.end());
        out_by_n = var_map["out_by_n"].as<string>();
        out_by_n.erase(std::remove(out_by_n.begin(), out_by_n.end(), '\"'), out_by_n.end());
        indexing_threads = var_map["indexing_threads"].as<int>();
    } catch (OpenConfigFileException &ex) {
        throw OpenConfigFileException(ex.what()); // Convert to our error type
    } catch (std::exception &ex) {
        throw OptionsParseException(ex.what());
    }
}