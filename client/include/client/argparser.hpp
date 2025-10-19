#pragma once

class ArgParser {
public:
    ArgParser(int argc, char** argv);

    bool isTestMode() const;

private:
    bool testMode_;
};
