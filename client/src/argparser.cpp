#include "client/argparser.hpp"

ArgParser::ArgParser(const QStringList& args)
    : testMode_(false)
{
    testMode_ = args.contains("--test");
}

bool ArgParser::isTestMode() const {
    return testMode_;
}