#pragma once
#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>
#include <vector>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Mutex.hpp>
#include "Worker.h"

namespace SExE
{
    using LoaderPaths = std::vector<std::pair<std::string, size_t>>;

    class FileLoader : public Worker
    {
        public:
        FileLoader();
        void addFile(const std::string& t_file);
        virtual void saveToFile(const std::string& t_file);

        size_t getTotalLines();
        size_t getCurrentLine();

        private:
        virtual bool processLine(std::stringstream& t_stream) = 0;
        virtual void resetForNextFile();
        void work();
        void countFileLines();

        LoaderPaths m_files;
        size_t m_totalLines;
        size_t m_currentLine;
    };
}

#endif // !FILELOADER_H