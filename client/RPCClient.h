#ifndef TEST_RPCCLIENT_H
#define TEST_RPCCLIENT_H

#include <string>
#include <unordered_map>
#include <zmq.hpp>
#include "ModelContainer.h"

class RPCClient {
 public:
  RPCClient();
  void connect(int container_id, std::string ip, int port);
  void disconnect(int container_id);
  void send_message(uint8_t* msg, size_t len, int container_id);
  /**  Terminates all active connections and exits **/
  void shutdown();

 private:
  void disconnect_socket(zmq::socket_t* socket, std::string address);
  std::string get_address(std::string ip, int port);

 private:
  std::unordered_map<int, ModelContainer>* active_connections;
  zmq::context_t context;
};

#endif TEST_RPCCLIENT_H