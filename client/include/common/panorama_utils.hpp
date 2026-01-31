// #ifndef PANORAMA_UTILS
// #define PANORAMA_UTILS

// #include <iostream>

// template <typename... Args>
// void pinfo(const Args&... args) {
//     (std::cout << "[INFO][Client]\t" << ... << args);
// }

// #endif

#ifndef __PANORAMA_UTILS_HPP__
#define __PANORAMA_UTILS_HPP__

#include <iostream>

template <typename... Args>
void pinfo(const Args&... args) {
    std::cout << "[INFO][Client]\t";
    (std::cout << ... << args);   // fold expression over operator<<
    std::cout << std::endl;
}

template <typename... Args>
void pdebug(const Args&... args) {
    std::cout << "[DEBUG][Client]\t";
    (std::cout << ... << args);   // fold expression over operator<<
    std::cout << std::endl;
}

#endif // __PANORAMA_UTILS_HPP__
