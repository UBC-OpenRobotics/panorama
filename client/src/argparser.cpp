#include "client/argparser.hpp"
#include "wx/cmdline.h"

ArgParser::ArgParser(int argc, char** argv)
    : testMode_(false), noGuiMode_(false), runtimeDir_("")
{
    wxCmdLineParser parser(argc, argv);
    parser.AddSwitch("t", "test", "test mode");
    parser.AddSwitch("n", "nogui", "run without GUI (console mode)");
    parser.AddOption("r", "runtime-dir", "runtime directory for data and config", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    parser.Parse(false); // don't exit on errors

    testMode_ = parser.Found("test");
    noGuiMode_ = parser.Found("nogui");

    wxString runtimeDirWx;
    if (parser.Found("runtime-dir", &runtimeDirWx)) {
        runtimeDir_ = runtimeDirWx.ToStdString();
    }
}

bool ArgParser::isTestMode() const {
    return testMode_;
}

bool ArgParser::isNoGuiMode() const {
    return noGuiMode_;
}

std::string ArgParser::getRuntimeDirectory() const {
    return runtimeDir_;
}