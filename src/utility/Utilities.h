#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>

namespace SExE
{
    namespace Utils
    {
        #ifdef _WIN32
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
        #include <shlwapi.h>

        #pragma comment (lib, "Shlwapi.lib")

        inline std::string getWorkingDirectory()
        {
            HMODULE hModule = GetModuleHandle(nullptr);
            if(!hModule)
            {
                return "";
            }
            char path[256];
            GetModuleFileName(hModule, path, sizeof(path));
            PathRemoveFileSpec(path);
            return std::string(path) + "\\";
        }

        inline std::vector<std::pair<std::string, bool>> getFileList(const std::string& t_directory, const std::string& t_search = "*.*", bool t_directories = false)
        {
            std::vector<std::pair<std::string, bool>> files;
            if(t_search.empty())
            {
                return files;
            }
            std::string path = t_directory + t_search;
            WIN32_FIND_DATA data;
            HANDLE found = FindFirstFile(path.c_str(), &data);
            if(found == INVALID_HANDLE_VALUE)
            {
                return files;
            }
            do
            {
                if(!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || t_directories)
                {
                    files.emplace_back(std::make_pair(
                        std::string(data.cFileName),
                        ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                    ));
                }
            }
            while(FindNextFile(found, &data));
            FindClose(found);
            return files;
        }

        #elif defined __linux__
        #include <unistd.h>
        #include <dirent.h>

        inline std::string getWorkingDirectory()
        {
            char cwd[1024];
            if(!getcwd(cwd, sizeof(cwd)))
            {
                return "";
            }
            return std::string(cwd) + std::string("/");
        }

        inline std::vector<std::pair<std::string, bool>> getFileList(const std::string& t_directory, const std::string& t_search = "*.*", bool t_directories = false)
        {
            std::vector<std::pair<std::string, bool>> files;

            DIR *dpdf;
            dpdf = opendir(t_directory.c_str());
            if(!dpdf)
            {
                return files;
            }
            if(t_search.empty())
            {
                return files;
            }
            std::string search = t_search;
            if(search[0] == '*')
            {
                search.erase(search.begin());
            }
            if(search[search.length() - 1] == '*')
            {
                search.erase(search.end());
            }
            struct dirent *epdf;
            while(epdf = readdir(dpdf))
            {
                std::string name = epdf->d_name;
                if(epdf->d_type == DT_DIR && !t_directories)
                {
                    continue;
                }
                if(t_search != "*.*")
                {
                    if(name.length() < search.length())
                    {
                        continue;
                    }
                    if(search[0] == '.')
                    {
                        if(name.compare(name.length() - search.length(), search.length(), search) != 0)
                        {
                            continue;
                        }
                    }
                    else if(name.find(search) == std::string::npos)
                    {
                        continue;
                    }
                }
                files.emplace_back(name, (epdf->d_type == DT_DIR));
            }
            closedir(dpdf);
            return files;
        }
        #endif

        inline void readQuotedString(std::stringstream& t_stream, std::string& t_string)
        {
            t_stream >> t_string;
            if(t_string.at(0) == '"')
            {
                while(t_string.at(t_string.length() - 1) != '"' || !t_stream.eof())
                {
                    std::string str;
                    t_stream >> str;
                    t_string.append(" " + str);
                }
            }

            t_string.erase(std::remove(t_string.begin(), t_string.end(), '"'), t_string.end());
        }

        template<class T>
        inline T interpolate(float tBegin, float tEnd, const T& begin_val, const T& end_val, float tX)
        {
            return static_cast<T>((((end_val - begin_val) / (tEnd - tBegin)) * (tX - tBegin)) + begin_val);
        }
    }
}

#endif // !UTILITIES_H
