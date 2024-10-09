#include "snappy.h"
#include <iostream>
#include <string>

int main()
{
    // Sample uncompressed data
    std::string uncompressed_data =
        "OpenAI's language models, such as GPT-3.5, are powerful tools for natural language understanding "
        "and generation. These models are trained on diverse datasets to capture the nuances of human "
        "language and can be used for a wide range of applications, from chatbots to content creation.";

    // Compress data
    std::string compressed_data;
    snappy::Compress(uncompressed_data.data(), uncompressed_data.size(), &compressed_data);

    // Print results
    // std::cout << "Original Data: " << uncompressed_data << std::endl;
    // std::cout << "Compressed Data: " << compressed_data << std::endl;
    // std::cout << compressed_data << std::endl;
    for (int i = 0; i < compressed_data.length(); i++)
    {
        if (compressed_data[i] < 0x20 || compressed_data[i] > 0x7E)
        {
            std::cout << ".";
        }
        else
        {
            std::cout << compressed_data[i];
        }
    }
    std::cout << std::endl;

    // Decompress data
    std::string decompressed_data;
    snappy::Uncompress(compressed_data.data(), compressed_data.size(), &decompressed_data);

    // Print results
    // std::cout << "Decompressed Data: " << decompressed_data << std::endl;

    return 0;
}
