#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;
namespace chrono = std::chrono;

template <typename TP>
std::time_t to_time_t(TP tp) {
    auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(tp - TP::clock::now() + chrono::system_clock::now());
    return chrono::system_clock::to_time_t(sctp);
}

int main(int argc, char** argv) {
    std::setlocale(LC_ALL, "en_US.UTF-8");
    bool console = argc != 3;

    std::cout << "CommandBackup 0.1 - Developed by GITHUB.COM/joaogabrielti." << std::endl << std::endl;

    std::string srcpath, dstpath;
    if (console) {
        std::cout << "Enter the source directory: ";
        std::cin >> srcpath;
        std::cout << "Enter the destination directory: ";
        std::cin >> dstpath;
    } else {
        srcpath = argv[1];
        dstpath = argv[2];
    }

    if (!fs::is_directory(srcpath)) {
        std::cout << "Invalid source directory!" << std::endl << std::endl;
        if (console) system("pause");
        return -1;
    } else if (!fs::is_directory(dstpath)) {
        std::cout << "Invalid destination directory!" << std::endl << std::endl;
        if (console) system("pause");
        return -1;
    }

    if (console) std::cout << std::endl;
    std::cout << "Copying files..." << std::endl;
    std::cout << "[" << srcpath << "] -> [" << dstpath << "]" << std::endl << std::endl;

    int i = 0;
    for (const auto& entry : fs::directory_iterator(srcpath)) {
        try {
            time_t tt = to_time_t(entry.last_write_time());
            struct tm* dt = (struct tm*) malloc(sizeof(struct tm));
            gmtime_s(dt, &tt);

            std::string month;
            int tmp = dt->tm_mon + 1;
            if (tmp < 10) month = "0" + std::to_string(tmp);
            else month = std::to_string(tmp);
            std::string year = std::to_string(dt->tm_year + 1900);

            time_t systime = chrono::system_clock::to_time_t(chrono::system_clock::now());
            struct tm* sdt = (struct tm*)malloc(sizeof(struct tm));
            gmtime_s(sdt, &systime);

            bool flag = (dt->tm_mday == sdt->tm_mday && dt->tm_mon == sdt->tm_mon && dt->tm_year == sdt->tm_year);
            free(dt);
            free(sdt);

            if (flag) {
                fs::path from = entry.path();
                fs::path topath = dstpath + "\\" + year + month;
                if (!fs::exists(topath) || !fs::is_directory(topath))
                    fs::create_directory(topath);

                fs::path to = topath.string() + "\\" + entry.path().filename().string();

                fs::copy_file(from, to);

                i++;
            }
        } catch (std::filesystem::filesystem_error& e) {
            std::cout << "CommandBackup cannot copy this file: " << entry.path().filename() << std::endl;
        }
    }

    std::cout << "Done! " << i << " files copied." << std::endl;
    if (console) system("pause");
}