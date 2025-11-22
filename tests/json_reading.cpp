#include "../client/include/rapidjson/document.h"
#include "../client/include/rapidjson/filereadstream.h"
#include <iostream>

int main()
{
    // Open the file for reading
    FILE* fp = fopen("telemetry.json", "r");

    if (!fp) {
        std::cerr << "Failed to open telemetry.json\n";
        return 1;
    }

    std::cout << "READ" << std::endl;
    // Use a FileReadStream to
      // read the data from the file
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer,
                                 sizeof(readBuffer));

    // Parse the JSON data 
      // using a Document object
    rapidjson::Document d;
    d.ParseStream(is);

    // Close the file
    fclose(fp);

    // Access the data in the JSON document (arrays)
    if (d.HasMember("temperature") && d["temperature"].IsArray()) {
    const auto& temps = d["temperature"]; //take the temperature array
    for (const auto& entry : temps.GetArray()) { //for every object in the array
        std::cout << "Time: " << entry["time"].GetString()
                  << " | Ambient: " << entry["t ambient"].GetString()
                  << " | CPU: " << entry["t cpu"].GetString()
                  << std::endl;
    }
}


    return 0;
}