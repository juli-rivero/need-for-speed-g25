#pragma once

#include "common/protocol.h"
#include "common/socket.h"
#include "gtest/gtest.h"

class ProtocolTest : public ::testing::Test {
   protected:
    Socket acceptor;
    Protocol client_protocol;
    Protocol server_protocol;

    ProtocolTest()
        : acceptor("7649"),
          client_protocol("localhost", "7649"),
          server_protocol(acceptor.accept()) {}
};
