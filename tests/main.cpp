#include "server.h"
#include <QCoreApplication>
#include <gtest/gtest.h>



TEST(TServer, ConnectionToRabbitMQ)
{
    TServer server;
    ASSERT_EQ(0, server.connectRabbit());
}

TEST(TServer, doubleMessage)
{
    TServer server;
    server.connectRabbit();
    TestTask::Messages::Request messageRequest;
    TestTask::Messages::Response messageResponse;
    messageRequest.set_id("id-1");
    messageRequest.set_req(12);
    std::string serializedMessage;
    messageRequest.SerializeToString(&serializedMessage);
    std::string serializedMessageToSend;
    serializedMessageToSend = server.doubleMessage(serializedMessage);
    messageResponse.ParseFromString(serializedMessageToSend);
    ASSERT_EQ(24, messageResponse.res());
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    testing::InitGoogleTest();
    if(RUN_ALL_TESTS())
    {
        return 1;
    }
    return a.exec();
}
