#pragma once
#include <QStringList>

class ArgParser {
public:
    ArgParser(const QStringList& args);

    bool isTestMode() const;

private:
    bool testMode_;
};
