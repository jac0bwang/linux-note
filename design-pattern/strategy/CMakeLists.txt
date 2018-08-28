file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_executable(stategy ${SOURCE_FILES})
#target_include_directories(stategy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
