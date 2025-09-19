#include <cstdlib>
#include <iostream>

int main() {
    std::cout << "[TEST] Launching panorama-client..." << std::endl;

    int ret = std::system("../client/panorama-client --test > /dev/null 2>&1");

    if (ret != 0) {
        std::cerr << "[FAIL] panorama-client did not run correctly." << std::endl;
        return 1;
    }

    std::cout << "[PASS] panorama-client runs successfully." << std::endl;
    return 0;
}
