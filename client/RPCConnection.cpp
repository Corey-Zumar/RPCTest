#include <zmq.hpp>
#include "RPCConnection.h"
#include "RPCUtil.h"

RPCConnection::RPCConnection(std::string address) {
  outbound_queue = std::shared_ptr<std::queue<OutboundMessage>>(new std::queue<OutboundMessage>());
  callbacks = std::shared_ptr<std::unordered_map<std::string, std::function<void(uint8_t *, size_t)> *>>(
      new std::unordered_map<std::string, std::function<void(uint8_t *, size_t)> *>());
  queue_lock = std::shared_ptr<std::mutex>(new std::mutex());
  connection_thread =
      std::thread(&RPCConnection::manage_connection, address, &interrupted, queue_lock, outbound_queue, callbacks);
}

RPCConnection::~RPCConnection() {
  outbound_queue.reset();
  callbacks.reset();
  queue_lock.reset();
}

void RPCConnection::queue_message(uint8_t *msg_content,
                                  size_t msg_len,
                                  std::function<void(uint8_t *, size_t)> *msg_callback) {
  queue_lock->lock();
  outbound_queue->push(OutboundMessage(msg_content, msg_len, msg_callback));
  queue_lock->unlock();
}

void RPCConnection::shutdown() {
  interrupted = true;
}

void RPCConnection::manage_connection(std::string address,
                                      bool* interrupted,
                                      std::shared_ptr<std::mutex> queue_lock,
                                      std::shared_ptr<std::queue<OutboundMessage>> outbound_messages,
                                      std::shared_ptr<std::unordered_map<std::string,
                                                                         std::function<void(uint8_t *,
                                                                                            size_t)> *>> callbacks) {
  zmq::socket_t socket(RPCUtil::context, ZMQ_DEALER);
  socket.connect(address);
  int msg_num = 0;
  zmq_pollitem_t items[] = {{socket, 0, ZMQ_POLLIN, 0}};
  while (true) {
    if (*interrupted) {
      RPCConnection::stop(&socket, address);
      return;
    }
    // If we find that the socket is subjected to excessively high load,
    // as determined by the high water mark (see HWM in ZeroMQ docs),
    // we may want to use a timeout for our poll
    zmq_poll(items, 1, 0);
    if (items[0].revents & ZMQ_POLLIN) {
      RPCConnection::receive_message(&socket, callbacks);
    }
    RPCConnection::send_messages(&socket, queue_lock, outbound_messages, callbacks, &msg_num);
  }
}

void RPCConnection::receive_message(
    zmq::socket_t *socket,
    std::shared_ptr<std::unordered_map<std::string, std::function<void(uint8_t *, size_t)> *>> callbacks) {
  zmq::message_t delimiter;
  zmq::message_t id_msg;
  zmq::message_t content;
  socket->recv(&delimiter, 0);
  socket->recv(&id_msg, 0);
  socket->recv(&content, 0);
  char *raw_id = (char *) id_msg.data();
  raw_id[id_msg.size()] = '\0';
  std::string id = std::string(raw_id);
  std::unordered_map<std::string, std::function<void(uint8_t *, size_t)> *>::const_iterator
      callback = callbacks->find(id);
  if (callback != callbacks->end() && callback->second) {
    callback->second->operator()((uint8_t *) content.data(), content.size());
    callbacks->erase(id);
  }
}

void RPCConnection::send_messages(zmq::socket_t *socket,
                                  std::shared_ptr<std::mutex> queue_lock,
                                  std::shared_ptr<std::queue<OutboundMessage>> outbound_messages,
                                  std::shared_ptr<std::unordered_map<std::string,
                                                                     std::function<void(uint8_t *,
                                                                                        size_t len)> *>> callbacks,
                                  int *msg_id) {
  if (outbound_messages->empty()) {
    return;
  }
  queue_lock->lock();
  while (!outbound_messages->empty()) {
    OutboundMessage msg = outbound_messages->front();
    std::string id = std::to_string(*msg_id);
    socket->send("", 0, ZMQ_SNDMORE);
    socket->send(id.c_str(), id.length(), ZMQ_SNDMORE);
    socket->send(msg.content, msg.len - 1, 0);
    callbacks->emplace(id, msg.callback);
    outbound_messages->pop();
    *msg_id = *msg_id + 1;
  }
  queue_lock->unlock();
}

void RPCConnection::stop(zmq::socket_t *socket, std::string address) {
  socket->disconnect(address);
  socket->close();
}
