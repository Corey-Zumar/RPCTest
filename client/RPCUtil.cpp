#include "RPCUtil.h"

zmq::context_t RPCUtil::context = zmq::context_t(1);

const std::string RPCUtil::get_address(std::string ip, uint16_t port) {
  return "tcp://" + ip + ":" + std::to_string(port);
}

void RPCUtil::log_millis(std::string tag) {
  std::chrono::milliseconds curr_time = std::chrono::duration_cast<std::chrono::milliseconds >(
      std::chrono::system_clock::now().time_since_epoch());
  std::string timestamp = tag + ": " + std::to_string(curr_time.count()) + "\n";
  std::printf(timestamp.c_str());
}
