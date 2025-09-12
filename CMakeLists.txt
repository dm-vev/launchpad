# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(launchpad)

littlefs_create_partition_image(boot boot FLASH_IN_PROJECT)
littlefs_create_partition_image(root root FLASH_IN_PROJECT)