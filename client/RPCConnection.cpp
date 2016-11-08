#include <zmq.hpp>
#include "RPCConnection.h"
#include "RPCUtil.h"

using namespace std;
using namespace zmq;

RPCConnection::RPCConnection(string address, function<void(bool)> connection_callback) :
    outbound_queue(queue<OutboundMessage>()),
    callbacks(unordered_map<string, function<void(uint8_t *, size_t)>>()) {
  thread(&RPCConnection::manage_connection,
         address,
         ref(interrupted),
         connection_callback,
         ref(queue_lock),
         ref(outbound_queue),
         ref(callbacks)).detach();
}

RPCConnection::~RPCConnection() {
  shutdown();
}

void RPCConnection::queue_message(uint8_t *msg_content,
                                  size_t msg_len,
                                  function<void(uint8_t *, size_t)> msg_callback) {
  queue_lock.lock();
  outbound_queue.push(OutboundMessage(msg_content, msg_len, msg_callback));
  queue_lock.unlock();
}

void RPCConnection::shutdown() {
  interrupted = true;
}

void RPCConnection::manage_connection(string address,
                                      const bool &interrupted,
                                      function<void(bool)> connection_callback,
                                      mutex &queue_lock,
                                      queue<OutboundMessage> &outbound_messages,
                                      unordered_map<string, function<void(uint8_t *, size_t)>> &callbacks) {
  socket_t socket(RPCUtil::context, ZMQ_DEALER);
  socket.connect(address);
  if (connection_callback) {
    connection_callback(true);
  }
  int msg_num = 0;
  zmq_pollitem_t items[] = {{socket, 0, ZMQ_POLLIN, 0}};
  while (true) {
    if (interrupted) {
      RPCConnection::stop(socket, address);
      return;
    }
    // If we find that the socket is subjected to excessively high load,
    // as determined by the high water mark (see HWM in ZeroMQ docs),
    // we may want to use a timeout for our poll
    zmq_poll(items, 1, 0);
    if (items[0].revents & ZMQ_POLLIN) {
      RPCConnection::receive_message(socket, callbacks);
    }
    RPCConnection::send_messages(socket, queue_lock, outbound_messages, callbacks, msg_num);
  }
}

void RPCConnection::receive_message(
    socket_t &socket,
    unordered_map<string, function<void(uint8_t *, size_t)>> &callbacks) {
  message_t delimiter;
  message_t id_msg;
  message_t content;
  socket.recv(&delimiter, 0);
  socket.recv(&id_msg, 0);
  socket.recv(&content, 0);
  char *raw_id = (char *) id_msg.data();
  raw_id[id_msg.size()] = '\0';
  string id = string(raw_id);
  unordered_map<string, function<void(uint8_t *, size_t)>>::const_iterator
      callback = callbacks.find(id);
  if (callback != callbacks.end() && callback->second) {
    callback->second((uint8_t *) content.data(), content.size());
    callbacks.erase(id);
  }
}

void RPCConnection::send_messages(socket_t &socket,
                                  mutex &queue_lock,
                                  queue<OutboundMessage> &outbound_messages,
                                  unordered_map<string, function<void(uint8_t *, size_t len)>> &callbacks,
                                  int &msg_id) {
  if (outbound_messages.empty()) {
    return;
  }
  queue_lock.lock();
  while (!outbound_messages.empty()) {
    OutboundMessage msg = outbound_messages.front();
    string id = to_string(msg_id);
    socket.send("", 0, ZMQ_SNDMORE);
    socket.send(id.c_str(), id.length(), ZMQ_SNDMORE);
    socket.send(msg.content, msg.len - 1, 0);
    callbacks.emplace(id, msg.callback);
    outbound_messages.pop();
    msg_id++;
    printf("%d\n", msg_id);
  }
  queue_lock.unlock();
}

void RPCConnection::stop(socket_t &socket, string address) {
  socket.disconnect(address);
  socket.close();
}
