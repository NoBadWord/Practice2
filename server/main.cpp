#include "server.h"

#include <gtest/gtest.h>

TEST(ConnectionTest, hi)
{
    TServer server;
    ASSERT_EQ(0,server.connectRabbit());
}

int main() {
    testing::InitGoogleTest();
    if(RUN_ALL_TESTS())
    {
        return 1;
    }
    TServer server;
    if (server.connectRabbit())
    {
        return 2;
    }
    for(;;)
    {
        server.consumeAndSendMessage();
    }
    return 0;
}
