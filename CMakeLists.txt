project(threadPoll)
find_package( Threads )
add_executable(threadPoll main.cpp)
target_link_libraries( threadPoll ${CMAKE_THREAD_LIBS_INIT} )