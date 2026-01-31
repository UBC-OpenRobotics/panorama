#pragma once

#include <string>

class ArgParser {
public:
    ArgParser(int argc, char** argv);

    bool isTestMode() const;
    bool isNoGuiMode() const;
    std::string getRuntimeDirectory() const;

private:
    bool testMode_;
    bool noGuiMode_;
    std::string runtimeDir_;
};
