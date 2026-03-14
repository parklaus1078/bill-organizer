# Test configuration for BillOrganizer

# Enable Qt Test
find_package(Qt6 REQUIRED COMPONENTS Test)

# Collect all source files from main application (excluding main.cpp)
file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/models/*.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/*.cpp
    ${CMAKE_SOURCE_DIR}/src/views/*.cpp
)

# Remove main.cpp from the list (it has main() function which conflicts with tests)
list(FILTER APP_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Function to add Qt tests
function(add_qt_test test_name)
    add_executable(${test_name} ${ARGN} ${APP_SOURCES})
    target_link_libraries(${test_name}
        Qt6::Test
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
    )
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    add_test(NAME ${test_name} COMMAND ${test_name})

    # Set test to run with offscreen platform for headless execution
    set_tests_properties(${test_name} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endfunction()

# Enable testing
enable_testing()

# Add unit tests
message(STATUS "Adding unit tests for PLAN-001...")
add_qt_test(tst_payment_card unit/tst_payment_card.cpp)
add_qt_test(tst_card_encryption unit/tst_card_encryption.cpp)
add_qt_test(tst_payment_card_manager_validation unit/tst_payment_card_manager_validation.cpp)

# Add integration tests
message(STATUS "Adding integration tests for PLAN-001...")
add_qt_test(tst_card_manager_integration integration/tst_card_manager_integration.cpp)

# Add UI tests
message(STATUS "Adding UI tests for PLAN-001...")
add_qt_test(tst_card_workflows ui/tst_card_workflows.cpp)

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    COMMENT "Running all tests for PLAN-001 Payment Card Management..."
)

message(STATUS "Test configuration complete for PLAN-001.")
message(STATUS "Run 'ctest --output-on-failure' to execute tests.")
