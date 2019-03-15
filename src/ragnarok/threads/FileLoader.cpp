#include <fstream>
#include "ragnarok/threads/FileLoader.h"
#include "ragnarok/utilities/Utilities.h"
#include <SFML/Graphics.hpp>
#include <iostream>

namespace ragnarok
{
    FileLoader::FileLoader() : m_totalLines(0), m_currentLine(0)
    {}

    void FileLoader::AddFile(const std::string& t_file)
    {
        m_files.emplace_back(t_file, 0);
    }

    size_t FileLoader::GetTotalLines()
    {
        sf::Lock lock(m_mutex);
        return m_totalLines;
    }

    size_t FileLoader::GetCurrentLine()
    {
        sf::Lock lock(m_mutex);
        return m_currentLine;
    }

    void FileLoader::SaveToFile(const std::string& t_file)
    {}

    /* Virtual function which can be used if we need to clear the state
    of some internal data that needs to exist while a file is being loaded.*/
    void FileLoader::ResetForNextFile()
    {}

    void FileLoader::Work()
    {
        CountFileLines();

        if(!m_totalLines)
        {
            std::cout << "No lines to process." << std::endl;
            Done();
            return;
        }

        for(auto& path : m_files)
        {
            ResetForNextFile();
            std::ifstream file(path.first);
            std::string line;
            std::string name;
            auto linesLeft = path.second;

            while(std::getline(file, line))
            {
                {
                    sf::Lock lock(m_mutex);
                    ++m_currentLine;
                    --linesLeft;
                }

                if(line[0] == '|')
                {
                    continue;
                }

                std::stringstream keystream(line);

                if(!ProcessLine(keystream))
                {
                    std::cout << "File loader terminated due to an internal error." << std::endl;
                    {
                        sf::Lock lock(m_mutex);
                        m_currentLine += linesLeft;
                    }
                    break;
                }
            }

            file.close();
        }

        m_files.clear();

        Done();
    }

    void FileLoader::CountFileLines()
    {
        m_totalLines = 0;
        m_currentLine = 0;

        for(auto path = m_files.begin(); path != m_files.end();)
        {
            // Remove all empty paths
            if(path->first.empty())
            {
                path = m_files.erase(path);
                continue;
            }

            std::ifstream file(path->first);

            // Remove paths if file fails to open
            if(!file.is_open())
            {
                std::cerr << "Failed to load file: " << path->first << std::endl;
                path = m_files.erase(path);
                continue;
            }

            // Ignore empty lines
            file.unsetf(std::ios_base::skipws);

            {
                sf::Lock lock(m_mutex);
                path->second = static_cast<size_t>(std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n'));
                m_totalLines += path->second;
                std::cout << "Lines in file '" << path->first << "': " << path->second << "\n";
            }

            ++path;
            file.close();
        }
    }
}
