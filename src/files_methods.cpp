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

void checkZip(ThreadSafeQueue<ReadFile> &filesContents, ReadFile *file){

    printf("I`m here");
    struct archive *a;
    struct archive_entry *entry;
    int r;
    int64_t length;
    void *buf;
    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    printf("%s, %ld", (char *)file->buff, file->length);
    r = archive_read_open_memory(a, file->buff, file->length);
    printf("fail here");
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

            readFile.buff = std::move(buf);
            free(buf);
            readFile.content = "not empty";
            readFile.length = length;
            readFile.extension = ".zip";
            readFile.filename = name;
            //checkZip(filesContents, &readFile);
        }
    }

}

void readFiles(ThreadSafeQueue<fs::path> &paths, ThreadSafeQueue<ReadFile> &filesContents, int maxFileSize, TimePoint &timeReadingFinish) {
    auto path = paths.deque();

    struct archive *a;
    struct archive_entry *entry;
    int r;
    int64_t length;
    void *buf;


    while (path != fs::path("")) {
        if (fs::file_size(path) >= maxFileSize){
            path = paths.deque();
            continue;
        }

        if (path.extension() == ".txt") {
            ReadFile readFile;
            std::ifstream ifs(path);
            string content{(std::istreambuf_iterator<char>(ifs)),
                           (std::istreambuf_iterator<char>())};
            readFile.content = std::move(content);
            readFile.extension = path.extension();
            readFile.filename = path.filename();
            filesContents.enque(std::move(readFile));
        }
        else if (path.extension() == ".zip") {

            a = archive_read_new();
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);
            std::ifstream raw_file(path, std::ios::binary);
            raw_file.seekg(0, std::ios::end);
            size_t file_size = raw_file.tellg();

            r = archive_read_open_filename(a, path.c_str(), file_size);
            if (r != ARCHIVE_OK){
                exit(26);
            }
            while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                ReadFile readFile;
                std::string name = archive_entry_pathname(entry), ext;
                int p = name.find('.');
                ext = name.substr(p, name.size());
                name = name.substr(0, p);
                printf("ext - %s, name - %s\n", ext.c_str(), name.c_str());
                if (ext == ".txt") {
                    length = archive_entry_size(entry);
                    buf = malloc(length);
                    archive_read_data(a, buf, length);
                    printf("t - %s\n", (char *) buf);

                    std::string content{(char *)buf};
                    readFile.content = std::move(content);
                    free(buf);
                    readFile.extension = ".txt";
                    readFile.filename = name;
                    filesContents.enque(std::move(readFile));

                } else if (ext == ".zip" ) {
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
                    printf("Enque here he he\n");
                    try {
                        printf("Checking zip");
                        //checkZip(filesContents, &readFile);
                        printf("Zip Good");
                    } catch(std::error_code e){
                        std::cerr << "Error code " << e <<"checkZip fail" << std::endl;
                    }
                }

            }

        }
        path = paths.deque();
    }
    ReadFile emptyReadFile;
    filesContents.enque(emptyReadFile);
    paths.enque(fs::path(""));

    timeReadingFinish = get_current_time_fenced();
}