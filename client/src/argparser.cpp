#include "client/argparser.hpp"
#include "wx/cmdline.h"

ArgParser::ArgParser(int argc, char** argv)
    : testMode_(false)
{
    wxCmdLineParser parser(argc, argv);
    parser.AddSwitch("test", "test", "test mode");
    parser.Parse(false); // don't exit on errors

    testMode_ = parser.Found("test");
}

bool ArgParser::isTestMode() const {
    return testMode_;
}