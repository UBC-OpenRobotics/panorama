#include <iostream>
#include <cstdio>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

using namespace rapidjson;
using namespace std;

int main() {

    FILE* fp = fopen("example.json", "r");
    if (!fp) {
        cerr << "Unable to open example.json\n";
        return 1;
    }
    
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    Document doc;
    doc.ParseStream(is);
    fclose(fp);

    if (doc.HasParseError()) {
        cerr << "Parse error with example.json";
    }

    if (doc.HasMember("temp 1")) {
        cout << doc["temp 1"].GetInt() << endl;
    }

    return 0;

}