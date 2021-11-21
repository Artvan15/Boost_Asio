# Boost_Asio
Very simple client-server application to show basics of asynchronous approach using Boost.Asio library.
Program comes without Boost.Asio; You have to have it on your computer.

## Build
To build a project open CMakeLists.txt and:
1) Change path for <b>BOOST_ROOT</b> variable to your actual path to library;
2) Change path for <b>BOOST_INCLUDEDIR</b> variable to include directory. Usually it is BOOST_ROOT/boost;
3) Change path for <b>BOOST_LIBRARYDIR</b> variable to lib directory; (optional)
4) Use CMake to build a project. You can use GUI or CMake commands like:
```
cmake -S . -B .\out\build
```
```
cmake --build .\out\build
```

## Project
By default server use 80 port.
![Alt text](https://github.com/Artvan15/Boost_Asio/blob/master/images/gif.gif "Gif")


### Explanation on how async sending work
![Alt text](images/async.png?raw=true "Async")
