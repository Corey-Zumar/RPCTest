#ifndef TEST_RPCSERVER_H
#define TEST_RPCSERVER_H

#include <zmq.hpp>
#include <unordered_map>
#include "ModelConfigStore.h"
class RPCServer {
 public:
  RPCServer(ModelConfigStore* model_store);
  void start(std::string address);
  zmq::socket_t server_socket;
 private:
  void save_container(zmq::message_t* msg);
  void send_ack();
  ModelConfigStore* models;
  uint16_t next_container_id = 0;
};

#endif //TEST_RPCSERVER_H
