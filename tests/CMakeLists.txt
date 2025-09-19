enable_testing()

add_executable(test_bringup test_bringup.cpp)
add_test(NAME ClientBringUp COMMAND test_bringup)