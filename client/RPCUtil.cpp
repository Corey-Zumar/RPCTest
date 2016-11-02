#include "RPCUtil.h"

zmq::context_t RPCUtil::context = zmq::context_t(1);

const std::string RPCUtil::get_address(std::string ip, uint16_t port) {
  return "tcp://" + ip + ":" + std::to_string(port);
}
