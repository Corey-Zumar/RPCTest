#ifndef TEST_RPCCLIENT_H
#define TEST_RPCCLIENT_H

#include <string>
#include <unordered_map>
#include "ModelConfigStore.h"
#include "RPCConnection.h"

using namespace std;

class RPCClient {
 public:
  RPCClient(ModelConfigStore *model_store);
  void send_message(uint8_t *msg,
                    size_t len,
                    int container_id,
                    function<void(uint8_t *, size_t)> *callback);
  void connect(int container_id, function<void(bool)>* callback);
  void disconnect(int container_id);

 private:
  unordered_map<int, unique_ptr<RPCConnection>> connections;
  ModelConfigStore *models;
};

#endif TEST_RPCCLIENT_H