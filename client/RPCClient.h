#ifndef TEST_RPCCLIENT_H
#define TEST_RPCCLIENT_H

#include <string>
#include <unordered_map>
#include "ModelConfigStore.h"
#include "RPCConnection.h"

using namespace std;

class RPCClient {
 public:
  RPCClient(const ModelConfigStore &model_store);
  ~RPCClient();
  void send_message(uint8_t *msg,
                    size_t len,
                    int container_id,
                    function<void(uint8_t *, size_t)> callback);
  void connect(int container_id, function<void(bool)> callback);
  void disconnect(int container_id);

 private:
  RPCClient(const RPCClient &);
  RPCClient &operator=(const RPCClient &);
  const ModelConfigStore &models;
  unordered_map<int, RPCConnection> connections;
};

#endif TEST_RPCCLIENT_H