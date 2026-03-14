cmake_minimum_required(VERSION 3.16)
project(BillOrganizer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Gui
    Sql
)

# Application sources
set(APP_SOURCES
    src/main.cpp

    # Views
    src/views/main_window.h
    src/views/main_window.cpp
    src/views/card_list_view.h
    src/views/card_list_view.cpp
    src/views/card_form_dialog.h
    src/views/card_form_dialog.cpp

    # Models
    src/models/payment_card.h
    src/models/payment_card.cpp
    src/models/payment_card_manager.h
    src/models/payment_card_manager.cpp
    src/models/payment_card_list_model.h
    src/models/payment_card_list_model.cpp

    # Utilities
    src/utils/card_encryption.h
    src/utils/card_encryption.cpp
    src/utils/card_database.h
    src/utils/card_database.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${APP_SOURCES})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Sql
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
