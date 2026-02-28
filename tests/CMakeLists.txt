enable_testing()

add_executable(test_bringup test_bringup.cpp)
add_test(NAME ClientBringUp COMMAND test_bringup)

add_executable(json_reading json_reading.cpp)

# target_link_libraries(json_reading PRIVATE )
