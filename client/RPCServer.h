#ifndef TEST_RPCSERVER_H
#define TEST_RPCSERVER_H

#include <zmq.hpp>
#include <unordered_map>
#include "ModelConfigStore.h"

using namespace std;
using namespace zmq;

class RPCServer {
 public:
  RPCServer(ModelConfigStore &model_store, const function<void(int)> connection_callback);
  ~RPCServer();
  void start(const std::string &address);
  void stop();
 private:
  RPCServer(const RPCServer &);
  RPCServer &operator=(const RPCServer &);
  static void listen(const string &address,
                     ModelConfigStore &models,
                     const function<void(int)> connection_callback,
                     const bool &interrupted);
  static bool save_container(int container_id, const message_t &msg, ModelConfigStore &model_store);
  static void send_ack(socket_t &socket);
  static void shutdown(const string &address, socket_t &socket);
  thread server_thread;
  const function<void(int)> connection_callback;
  ModelConfigStore &models;
  bool interrupted = false;
};

#endif //TEST_RPCSERVER_H
