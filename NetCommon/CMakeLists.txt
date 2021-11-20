project(NetCommon)
set(HEADER_FILES Client.h Connection.h ConnectionClient.h
   ConnectionServer.h
    CustomMsgTypes.h
    IClient.h
    MessageWithConnection.h
    NetMessage.h
    Server.h
    ThreadGuard.h
    ThreadSafeQueue.h)

add_library(NetCommon main.cpp ${HEADER_FILES})

#include path to boost headers
if(Boost_FOUND)
    target_include_directories(NetCommon PUBLIC ${Boost_INCLUDE_DIRS})
endif()




    

    