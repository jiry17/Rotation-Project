cmake_minimum_required(VERSION 3.5)
project(ISToolExecutor)
cmake_policy(SET CMP0079 NEW)

add_executable(task_enumeration task_enumeration.cpp)
target_link_libraries(task_enumeration ${ALL_LIBS} ${ALL_LIBS} ${THIRDPARTY_LIBS})

add_executable(task_fta task_fta.cpp)
target_link_libraries(task_fta ${ALL_LIBS} ${ALL_LIBS} ${THIRDPARTY_LIBS})