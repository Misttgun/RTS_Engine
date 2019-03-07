#pragma once
#ifndef HELPERS_H
#define HELPERS_H

#include <SFML/Graphics/Text.hpp>
#include <fstream>

namespace SExE
{
    namespace Utils
    {
        inline float getSFMLTextMaxHeight(const sf::Text& t_text)
        {
            auto charSize = t_text.getCharacterSize();
            auto font = t_text.getFont();
            auto string = t_text.getString().toAnsiString();
            bool bold = (t_text.getStyle() & sf::Text::Bold);
            float max = 0.f;
            for(size_t i = 0; i < string.length(); ++i)
            {
                sf::Uint32 character = string[i];
                auto glyph = font->getGlyph(character, charSize, bold);
                auto height = glyph.bounds.height;
                if(height <= max)
                {
                    continue;
                }
                max = height;
            }
            return max;
        }

        inline void centerSFMLText(sf::Text& t_text)
        {
            sf::FloatRect rect = t_text.getLocalBounds();
            auto maxHeight = Utils::getSFMLTextMaxHeight(t_text);
            t_text.setOrigin(rect.left + (rect.width * 0.5f), rect.top + ((maxHeight >= rect.height ? maxHeight * 0.5f : rect.height * 0.5f)));
        }

        inline std::string readFile(const std::string& t_filename)
        {
            std::ifstream file(t_filename);
            if(!file.is_open())
            {
                return "";
            }
            std::string output;
            std::string line;
            while(std::getline(file, line))
            {
                output.append(line + "\n");
            }
            file.close();
            return output;
        }

        inline void sortFileList(std::vector<std::pair<std::string, bool>>& t_list)
        {
            std::sort(t_list.begin(), t_list.end(), [](std::pair<std::string, bool>& t_1, std::pair<std::string, bool>& t_2)
            {
                if(t_1.second && !t_2.second)
                {
                    return true;
                }
                return false;
            }
            );
        }
    }
}

#endif // !HELPERS_H
