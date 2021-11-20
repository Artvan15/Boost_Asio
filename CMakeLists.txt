cmake_minimum_required(VERSION 3.10)

project(Boost_Asio VERSION 1.0)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#set(ENV{BOOST_ROOT}["C:\\programs2\\boost_1_75_0"])
#set(ENV{BOOST_INCLUDEDIR}["C:\\programs2\\boost_1_75_0\\boost"])
#set(ENV{BOOST_LIBRARYDIR}["C:\\programs2\\boost_1_75_0\\lib64-msvc-14.2"])

#set cache value where find boost. If found, special values will be filled
set(BOOST_ROOT "C:\\programs2\\boost_1_75_0" CACHE PATH "Boost root" [FORCE])
set(BOOST_INCLUDEDIR "C:\\programs2\\boost_1_75_0\\boost" CACHE PATH "Boost include" [FORCE])
set(BOOST_LIBRARYDIR "C:\\programs2\\boost_1_75_0\\lib64-msvc-14.2" CACHE PATH "Boost lib" [FORCE])
find_package(Boost 1.75.0 COMPONENTS system)

#add projects to solution
add_subdirectory(NetCommon)
add_subdirectory(Client)
add_subdirectory(Server)



