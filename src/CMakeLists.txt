cmake_minimum_required(VERSION 3.5)

project(wvm_ui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17 -lstdc++fs")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(ANDROID)
  add_library(wvm_ui SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    createprojectwindow.cpp
    createprojectwindow.h
    createprojectwindow.ui
    projectwindow.cpp
    projectwindow.h
    projectwindow.ui
    chyperrectangle.cpp
    chyperrectangle.h
    cnode.cpp
    cnode.h
    crtree.cpp
    crtree.h
    doublevector.cpp
    doublevector.h
    cnotrtree.cpp
    cnotrtree.h
    rtreetest.cpp
    rtreetest.h
  )
else()
  add_executable(wvm_ui WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    createprojectwindow.cpp
    createprojectwindow.h
    createprojectwindow.ui
    projectwindow.cpp
    projectwindow.h
    projectwindow.ui
    chyperrectangle.cpp
    chyperrectangle.h
    cnode.cpp
    cnode.h
    crtree.cpp
    crtree.h
    doublevector.cpp
    doublevector.h
    cnotrtree.cpp
    cnotrtree.h
    rtreetest.cpp
    rtreetest.h
  )
endif()

target_link_libraries(wvm_ui PRIVATE Qt5::Widgets stdc++fs)
