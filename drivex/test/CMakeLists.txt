hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)
add_executable(TestFuse TestFuse.cpp)
target_link_libraries(TestFuse GTest::main libdrivex)
set_target_properties(TestFuse PROPERTIES RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestFuse
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestFuse)
