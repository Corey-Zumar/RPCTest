#ifndef TEST_RPCSERVER_H
#define TEST_RPCSERVER_H

#include <zmq.hpp>
#include <unordered_map>
#include "ModelConfigStore.h"

using namespace std;
using namespace zmq;

class RPCServer {
 public:
  RPCServer(ModelConfigStore *model_store, function<void(int)> *connection_callback);
  ~RPCServer();
  void start(std::string address);
  void stop();
 private:
  static void listen(string address,
                     ModelConfigStore *model_store,
                     function<void(int)> *connection_callback,
                     bool *interrupted);
  static bool save_container(int container_id, message_t *msg, ModelConfigStore *model_store);
  static void send_ack(socket_t *socket);
  static void shutdown(string address, socket_t *socket);
  thread server_thread;
  function<void(int)> *connection_callback;
  ModelConfigStore *models;
  bool interrupted = false;
};

#endif //TEST_RPCSERVER_H
