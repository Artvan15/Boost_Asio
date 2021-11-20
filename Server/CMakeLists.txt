project(Server)

add_executable(Server Server.cpp)

if(Boost_FOUND)
    target_include_directories(Server PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_directories(Server PUBLIC ${Boost_LIBRARY_DIRS})
    target_link_libraries(Server PUBLIC ${Boost_LIBRARIES})
endif()

target_include_directories(Server PUBLIC ${Boost_Asio_SOURCE_DIR}/NetCommon)
target_compile_features(Server PUBLIC cxx_std_17)