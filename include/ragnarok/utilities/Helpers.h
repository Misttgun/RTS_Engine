#pragma once

#include <SFML/Graphics/Text.hpp>
#include <fstream>

namespace ragnarok
{
    namespace Utils
    {
        inline float GetSFMLTextMaxHeight(const sf::Text& t_text)
        {
            const auto charSize = t_text.getCharacterSize();
            const auto font = t_text.getFont();
            auto string = t_text.getString().toAnsiString();
            const bool bold = (t_text.getStyle() & sf::Text::Bold);
            float max = 0.f;

            for (sf::Uint32 character : string)
            {
                const auto glyph = font->getGlyph(character, charSize, bold);
                const auto height = glyph.bounds.height;

                if(height <= max)
                {
                    continue;
                }

                max = height;
            }

            return max;
        }

        inline void CenterSFMLText(sf::Text& t_text)
        {
            const sf::FloatRect rect = t_text.getLocalBounds();
            const auto maxHeight = Utils::GetSFMLTextMaxHeight(t_text);
            t_text.setOrigin(rect.left + (rect.width * 0.5f), rect.top + ((maxHeight >= rect.height ? maxHeight * 0.5f : rect.height * 0.5f)));
        }

        inline std::string ReadFile(const std::string& t_filename)
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

        inline void SortFileList(std::vector<std::pair<std::string, bool>>& t_list)
        {
            std::sort(t_list.begin(), t_list.end(), [](std::pair<std::string, bool>& t_1, std::pair<std::string, bool>& t_2) 
            {
                return t_1.second && !t_2.second;
            });
        }
    }
}