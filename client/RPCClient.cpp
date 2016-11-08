#include <thread>
#include "RPCClient.h"

using namespace std;

RPCClient::RPCClient(const ModelConfigStore &model_store) :
    models(model_store), connections(unordered_map<int, RPCConnection>()) {

}

RPCClient::~RPCClient() {
  for (auto &entry : connections) {
    entry.second.shutdown();
  }
}

void RPCClient::send_message(uint8_t *msg,
                             size_t len,
                             int container_id,
                             function<void(uint8_t *, size_t)> callback) {
  unordered_map<int, RPCConnection>::iterator connection = connections.find(container_id);
  if (connection != connections.end()) {
    connection->second.queue_message(msg, len, callback);
  }
}

void RPCClient::connect(int container_id, function<void(bool)> callback) {
  const ModelContainer *model = models.get(container_id);
  if (!model) {
    if (callback) { // What does this do now???
      callback(false);
    }
    return;
  }
  connections.emplace(piecewise_construct, forward_as_tuple(container_id), forward_as_tuple(model->address, callback));
}

void RPCClient::disconnect(int container_id) {
  unordered_map<int, RPCConnection>::iterator connection = connections.find(container_id);
  if (connection != connections.end()) {
    connection->second.shutdown();
    connections.erase(container_id);
  }
}
