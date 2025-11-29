#include "client/argparser.hpp"
#include "wx/cmdline.h"

ArgParser::ArgParser(int argc, char** argv)
    : testMode_(false), noGuiMode_(false)
{
    wxCmdLineParser parser(argc, argv);
    parser.AddSwitch("test", "test", "test mode");
    parser.AddSwitch("nogui", "nogui", "run without GUI (console mode)");
    parser.Parse(false); // don't exit on errors

    testMode_ = parser.Found("test");
    noGuiMode_ = parser.Found("nogui");
}

bool ArgParser::isTestMode() const {
    return testMode_;
}

bool ArgParser::isNoGuiMode() const {
    return noGuiMode_;
}