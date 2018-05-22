add_library(fmt-header-only INTERFACE)
target_include_directories(fmt-header-only INTERFACE include)
target_compile_definitions(fmt-header-only INTERFACE FMT_HEADER_ONLY=1)

print_interface_properties(fmt-header-only)