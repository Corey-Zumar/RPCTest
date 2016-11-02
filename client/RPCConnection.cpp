#include <zmq.hpp>
#include "RPCConnection.h"
#include "RPCUtil.h"

RPCConnection::RPCConnection(std::string address) {
  outbound_queue = std::shared_ptr<std::queue<OutboundMessage>>(new std::queue<OutboundMessage>());
  callbacks = std::shared_ptr<std::unordered_map<int, std::function<void(uint8_t *)> *>>(
      new std::unordered_map<int, std::function<void(uint8_t *)> *>());
  connection_thread = std::thread(&RPCConnection::manage_connection, address, outbound_queue, callbacks);
}

void RPCConnection::queue_message(uint8_t *msg_content, size_t msg_len, std::function<void(uint8_t *)> *msg_callback) {
  // MODIFY TO ACQUIRE A LOCK!!!
  outbound_queue->push(OutboundMessage(msg_content, msg_len, msg_callback));
}

void RPCConnection::manage_connection(std::string address,
                                      std::shared_ptr<std::queue<OutboundMessage>> outbound_messages,
                                      std::shared_ptr<std::unordered_map<int,
                                                                         std::function<void(uint8_t *)> *>> callbacks) {
  zmq::socket_t socket(RPCUtil::context, ZMQ_DEALER);
  socket.connect(address);
  int msg_num = 0;
  zmq_pollitem_t items[] = {{socket, 0, ZMQ_POLLIN, 0}};
  while (true) {
    // If we find that the socket is subjected to excessively high load,
    // as determined by the high water mark (see HWM in ZeroMQ docs),
    // we may want to use a timeout for our poll
    zmq_poll(items, 1, 0);
    if (items[0].revents & ZMQ_POLLIN) {
      RPCConnection::receive_message(&socket, callbacks);
    }
    RPCConnection::send_messages(&socket, outbound_messages, callbacks, &msg_num);
  }
}

void RPCConnection::receive_message(
    zmq::socket_t *socket,
    std::shared_ptr<std::unordered_map<int, std::function<void(uint8_t *)> *>> callbacks) {
  zmq::message_t delimiter;
  zmq::message_t id_msg;
  zmq::message_t content;
  socket->recv(&delimiter, 0);
  socket->recv(&id_msg, 0);
  socket->recv(&content, 0);
  int id = ((int*) id_msg.data())[0];
  std::unordered_map<int, std::function<void(uint8_t *)> *>::const_iterator
      callback = callbacks->find(id);
  if (callback != callbacks->end() && callback->second) {
    callback->second->operator()((uint8_t *) content.data());
    callbacks->erase(id);
  }
}

void RPCConnection::send_messages(zmq::socket_t *socket,
                                  std::shared_ptr<std::queue<OutboundMessage>> outbound_messages,
                                  std::shared_ptr<std::unordered_map<int,
                                                                     std::function<void(uint8_t *)> *>> callbacks,
                                  int *msg_id) {
  while (!outbound_messages->empty()) {
    OutboundMessage msg = outbound_messages->front();
    socket->send("", 0, ZMQ_SNDMORE);
    socket->send(msg_id, sizeof(int), ZMQ_SNDMORE);
    socket->send(msg.content, msg.len - 1, 0);
    callbacks->emplace(*msg_id, msg.callback);
    outbound_messages->pop();
    *msg_id++;
  }
}
