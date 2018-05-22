cmake_minimum_required (VERSION 3.8)

project ("sb_minimum")

Include(ImportTargets.cmake)

# Include sub-projects.
add_subdirectory(fmt)
add_subdirectory(util)
add_subdirectory(sb_loader)
