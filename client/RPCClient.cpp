#include "RPCClient.h"

RPCClient::RPCClient() : context(zmq::context_t(1)) {
  active_connections = new std::unordered_map<int, ModelContainer>();
}

void RPCClient::connect(int container_id, std::string ip, int port) {
  if (active_connections->find(container_id) == active_connections->end()) {
    std::string container_address = get_address(ip, port);
    zmq::socket_t* socket = new zmq::socket_t(context, ZMQ_REQ);
    socket->connect(container_address);
    active_connections->emplace(container_id, ModelContainer(container_address, socket));
  }
}

void RPCClient::send_message(uint8_t *msg, size_t len, int container_id) {
  const std::unordered_map<int, ModelContainer>::const_iterator container = active_connections->find(container_id);
  if (container == active_connections->end()) {
    return;
  }
  zmq_send(*container->second.socket, msg, len, 0);
}

void RPCClient::shutdown() {
  for(auto entry : *active_connections) {
    entry.second.socket->disconnect(entry.second.address);
    entry.second.socket->close();
  }
}

void RPCClient::disconnect(int container_id) {
  const std::unordered_map<int, ModelContainer>::const_iterator entry = active_connections->find(container_id);
  if (entry != active_connections->end()) {
    entry->second.socket->disconnect(entry->second.address);
    entry->second.socket->close();
    active_connections->erase(entry);
  }
}

std::string RPCClient::get_address(std::string ip, int port) {
  return "tcp://" + ip + ":" + std::to_string(port);
}
