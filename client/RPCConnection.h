#ifndef TEST_RPCCONNECTION_H
#define TEST_RPCCONNECTION_H

#include <thread>
#include <string>
#include <queue>
#include <unordered_map>

using namespace std;
using namespace zmq;

class RPCConnection {
 public:
  RPCConnection(string address, function<void(bool)>* connection_callback);
  ~RPCConnection();
  void queue_message(uint8_t *msg_content, size_t msg_len, function<void(uint8_t *, size_t)> *msg_callback);
  void shutdown();

 private:
  class OutboundMessage {
   public:
    OutboundMessage(uint8_t *msg_content, size_t msg_len, function<void(uint8_t *, size_t)> *msg_callback) {
      content = msg_content;
      len = msg_len;
      callback = msg_callback;
    }
    uint8_t *content;
    size_t len;
    function<void(uint8_t *, size_t)> *callback;
  };
  shared_ptr<queue<OutboundMessage>> outbound_queue;
  shared_ptr<unordered_map<string, function<void(uint8_t *, size_t)> *>> callbacks;
  shared_ptr<mutex> queue_lock;
  thread connection_thread;
  bool interrupted = false;
  static void manage_connection(string address,
                                bool *interrupted,
                                function<void(bool)>* connection_callback,
                                shared_ptr<mutex> queue_lock,
                                shared_ptr<queue<OutboundMessage>> outbound_messages,
                                shared_ptr<unordered_map<string, function<void(uint8_t *, size_t)> *>> callbacks);
  static void receive_message(socket_t *socket,
                              shared_ptr<unordered_map<string, function<void(uint8_t *, size_t)> *>> callbacks);
  static void send_messages(socket_t *socket,
                            shared_ptr<mutex> queue_lock,
                            shared_ptr<queue<OutboundMessage>> outbound_messages,
                            shared_ptr<unordered_map<string, function<void(uint8_t *, size_t)> *>> callbacks,
                            int *msg_id);
  static void stop(socket_t *socket, string address);
};

#endif //TEST_RPCCONNECTION_H
