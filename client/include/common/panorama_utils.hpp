#ifndef PANORAMA_UTILS
#define PANORAMA_UTILS

#include <iostream>

template <typename... Args>
void pinfo(const Args&... args) {
    (std::cout << "[INFO][Client]\t" << ... << args);
}

#endif