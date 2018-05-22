# Wrap SBReader shared library in target
add_library(sb_reader SHARED IMPORTED)
set_property(TARGET sb_reader PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/lib)
if(UNIX)	
	set_property(TARGET sb_reader PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libSBReadFile.so)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib/libSBReadFile.so DESTINATION lib)
else()	
	set_property(TARGET sb_reader PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/SBReadFile.dll)
	set_property(TARGET sb_reader PROPERTY IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/SBReadFile.lib)
endif()

add_executable(mloader
	src/sb_loader.cpp
	src/sb_loader.h
)

target_link_libraries(mloader
	PRIVATE
		util
		sb_reader
		fmt-header-only)

if(UNIX)
	set_property(TARGET mloader PROPERTY INSTALL_RPATH \$ORIGIN/../lib)
endif()

install(TARGETS mloader
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
