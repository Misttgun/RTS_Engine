#pragma once

#include <string>
#include <vector>
#include "Worker.h"

namespace ragnarok
{
    using LoaderPaths = std::vector<std::pair<std::string, size_t>>;

    /* Base class for any file loading class.*/
    class FileLoader : public Worker
    {
        public:
        FileLoader();
        void AddFile(const std::string& t_file);
        virtual void SaveToFile(const std::string& t_file);

        size_t GetTotalLines();
        size_t GetCurrentLine();

        private:
        virtual bool ProcessLine(std::stringstream& t_stream) = 0;
        virtual void ResetForNextFile();

        // Do work on a different thread
        void Work() override;

        // Verify file validity and determine the amount of work ahead
        void CountFileLines();

        // All the paths added to the loader and their number of lines
        LoaderPaths m_files;
        size_t m_totalLines;
        size_t m_currentLine;
    };
}