get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} C CXX)

file(GLOB_RECURSE HEADERS "[^.]*.hpp")
file(GLOB_RECURSE SOURCES "[^.]*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    ${CMAKE_SOURCE_DIR}/Assets
#    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
# )

target_link_libraries(${PROJECT_NAME} LINK_PRIVATE nfd)
target_link_libraries(${PROJECT_NAME} LINK_PRIVATE raygui)
target_link_libraries(${PROJECT_NAME} LINK_PRIVATE raylib)

if(WIN32)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${SOLUTION_ROOT})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})