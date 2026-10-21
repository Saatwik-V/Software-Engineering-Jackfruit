#pragma once

#include <string>

namespace jackfruit::decompression {

class Decoder {
public:
    static bool decompressFile(const std::string& archivePath, const std::string& outputPath);
};

} // namespace jackfruit::decompression
