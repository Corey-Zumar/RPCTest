#include <thread>
#include "RPCClient.h"

RPCClient::RPCClient(ModelConfigStore *model_store) :
    connections(std::unordered_map<int, std::unique_ptr<RPCConnection>>()) {
  models = model_store;
}

void RPCClient::send_message(uint8_t *msg,
                             size_t len,
                             int container_id,
                             std::function<void(uint8_t *)> *callback) {
  std::unordered_map<int, std::unique_ptr<RPCConnection>>::const_iterator connection = connections.find(container_id);
  if(connection != connections.end()) {
    connection->second->queue_message(msg, len, callback);
  }
}

void RPCClient::connect(int container_id, std::function<void(bool)> *callback) {
  ModelContainer *model = models->get(container_id);
  if (!model) {
    if (callback) {
      callback->operator()(false);
    }
    return;
  }
  std::unique_ptr<RPCConnection> connection = std::unique_ptr<RPCConnection>(new RPCConnection(model->address));
  connections.emplace(container_id, std::move(connection));
}