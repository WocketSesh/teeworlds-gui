cmake_minimum_required(VERSION 3.12)

project(teeworlds-utils)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
pkg_check_modules(LIBSOUP REQUIRED libsoup-3.0)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0)

find_package(fmt REQUIRED)



include_directories(src)
include_directories(src/pages)


# set(SOURCES
#   src/pages/BasePage.cpp
#   src/pages/MainPage.cpp
#   src/pages/FriendsPage.cpp
#   src/pages/PageManager.cpp
#   src/pages/FriendsPage.h
#   src/pages/BasePage.h
#   src/pages/MainPage.h
#   src/pages/PageManager.h
#   src/main.cpp
#   )


file(GLOB_RECURSE SOURCES "src/*.cpp" "src/.h")


add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${GTKMM_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBSOUP_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${JSON_GLIB_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GTKMM_LIBRARIES} ${LIBSOUP_LIBRARIES} ${JSON_GLIB_LIBRARIES} fmt)




