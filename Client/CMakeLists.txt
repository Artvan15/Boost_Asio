project(Client)


add_executable(Client main.cpp)

#path to boost headers and libraries. Add libraries.
if(Boost_FOUND)  
    target_include_directories(Client PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_directories(Client PUBLIC ${Boost_LIBRARY_DIRS})
    target_link_libraries(Client PUBLIC ${Boost_LIBRARIES})
endif()

target_include_directories(Client PUBLIC ${Boost_Asio_SOURCE_DIR}/NetCommon)

#specify the c++17 standard
target_compile_features(Client PUBLIC cxx_std_17)
