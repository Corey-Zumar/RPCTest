#include <thread>
#include "RPCClient.h"

using namespace std;

RPCClient::RPCClient(const ModelConfigStore &model_store) :
    models(model_store), connections(unordered_map<int, unique_ptr<RPCConnection>>()) {

}

RPCClient::~RPCClient() {
//  for (auto entry : connections) {
//    stop(entry.second);
//  }
}

void RPCClient::send_message(uint8_t *msg,
                             size_t len,
                             int container_id,
                             function<void(uint8_t *, size_t)> callback) {
  unordered_map<int, unique_ptr<RPCConnection>>::const_iterator connection = connections.find(container_id);
  if (connection != connections.end()) {
    connection->second->queue_message(msg, len, callback);
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
  unique_ptr<RPCConnection> connection = unique_ptr<RPCConnection>(new RPCConnection(model->address, callback));
  connections.emplace(container_id, move(connection));
}

void RPCClient::disconnect(int container_id) {
  unordered_map<int, unique_ptr<RPCConnection>>::iterator connection = connections.find(container_id);
  if (connection != connections.end()) {
    stop(move(connection->second));
    connections.erase(container_id);
  }
}

void RPCClient::stop(unique_ptr<RPCConnection> connection) {
  connection->shutdown();
  connection.reset();
}
