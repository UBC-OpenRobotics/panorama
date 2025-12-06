#pragma once

class ArgParser {
public:
    ArgParser(int argc, char** argv);

    bool isTestMode() const;
    bool isNoGuiMode() const;

private:
    bool testMode_;
    bool noGuiMode_;
};
