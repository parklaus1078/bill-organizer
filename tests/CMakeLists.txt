# Test configuration for BillOrganizer

# Enable Qt Test
find_package(Qt6 REQUIRED COMPONENTS Test)

# Function to add Qt tests
function(add_qt_test test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name}
        Qt6::Test
        Qt6::Core
        Qt6::Widgets
    )
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add unit tests
# add_qt_test(tst_example unit/tst_example.cpp)

# Add integration tests
# add_qt_test(tst_integration integration/tst_integration.cpp)

# Add UI tests
# add_qt_test(tst_ui ui/tst_ui.cpp)
