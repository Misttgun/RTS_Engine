#pragma once

#include <string>

namespace ragnarok
{
    class MapLoadee
    {
        public:
        virtual ~MapLoadee() = default;
        virtual void ReadMapLine(const std::string& t_type, std::stringstream& t_stream) = 0;
        virtual void SaveMap(std::ofstream& t_fileStream) = 0;
    };
}