#include "server.h"
#include <QCoreApplication>
#include <gtest/gtest.h>

TEST(TServer, ConnectionToRabbitMQ)
{
    TServer server;
    ASSERT_EQ(0, server.connectRabbit());
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
