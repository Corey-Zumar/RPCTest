#ifndef TEST_RPCCLIENT_H
#define TEST_RPCCLIENT_H

#include <string>
#include <unordered_map>
#include "ModelConfigStore.h"
#include "RPCConnection.h"

class RPCClient {
 public:
  RPCClient(ModelConfigStore *model_store);
  void send_message(uint8_t *msg,
                    size_t len,
                    int container_id,
                    std::function<void(uint8_t *, size_t)> *callback);
  void connect(int container_id, std::function<void(bool)> *callback);
  void disconnect(int container_id);

 private:
  std::unordered_map<int, std::unique_ptr<RPCConnection>> connections;
  ModelConfigStore *models;
};

#endif TEST_RPCCLIENT_H