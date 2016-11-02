#ifndef TEST_RPCUTIL_H
#define TEST_RPCUTIL_H

#include <string>
#include <zmq.hpp>
class RPCUtil {
 public:
  static const std::string get_address(std::string ip, uint16_t port);
  static zmq::context_t context;
};

#endif //TEST_RPCUTIL_H
