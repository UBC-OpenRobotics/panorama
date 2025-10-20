#include <iostream>
#include <thread>
#include <chrono>

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <signal.h>
#endif

int main() {
    std::cout << "[TEST] Launching panorama-client..." << std::endl;

#if defined(_WIN32)
    const char* exePath = "../client/panorama-client.exe";
#elif defined(__APPLE__)
    const char* exePath = "../client/panorama-client.app/Contents/MacOS/panorama-client";
#else
    const char* exePath = "../client/panorama-client";
#endif

    const char* arg = "--test";

#if defined(_WIN32)
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    std::string cmd = std::string(exePath) + " " + arg;

    if (!CreateProcessA(
            nullptr,
            cmd.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi)) {
        std::cerr << "[FAIL] Failed to launch panorama-client." << std::endl;
        return 1;
    }

    DWORD waitResult = WaitForSingleObject(pi.hProcess, 5000);
    if (waitResult == WAIT_TIMEOUT) {
        TerminateProcess(pi.hProcess, 1);
        std::cerr << "[FAIL] panorama-client did not exit within 5 seconds." << std::endl;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (exitCode != 0) {
        std::cerr << "[FAIL] panorama-client exited with code " << exitCode << std::endl;
        return 1;
    }

#else
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        execl(exePath, exePath, arg, (char*)nullptr);
        std::exit(127); // exec failed
    } else if (pid < 0) {
        std::cerr << "[FAIL] Failed to fork process." << std::endl;
        return 1;
    }

    int status = 0;
    const int timeout_ms = 5000;
    const int interval_ms = 100;

    int elapsed = 0;
    while (elapsed < timeout_ms) {
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == pid) {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                std::cout << "[PASS] panorama-client exited normally." << std::endl;
                return 0;
            } else {
                std::cerr << "[FAIL] panorama-client exited abnormally." << std::endl;
                return 1;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        elapsed += interval_ms;
    }

    kill(pid, SIGKILL);
    std::cerr << "[FAIL] panorama-client did not exit within 5 seconds." << std::endl;
    return 1;
#endif

    std::cout << "[PASS] panorama-client runs successfully." << std::endl;
    return 0;
}
