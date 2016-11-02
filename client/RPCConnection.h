#ifndef TEST_RPCCONNECTION_H
#define TEST_RPCCONNECTION_H

#include <thread>
#include <string>
#include <queue>
#include <unordered_map>

class RPCConnection {
 public:
  RPCConnection(std::string address);
  void queue_message(uint8_t *msg_content, size_t msg_len, std::function<void(uint8_t *)> *msg_callback);

 private:
  class OutboundMessage {
   public:
    OutboundMessage(uint8_t *msg_content, size_t msg_len, std::function<void(uint8_t *)> *msg_callback) {
      content = msg_content;
      len = msg_len;
      callback = msg_callback;
    }
    uint8_t *content;
    size_t len;
    std::function<void(uint8_t *)> *callback;
  };
  std::shared_ptr<std::queue<OutboundMessage>> outbound_queue;
  std::shared_ptr<std::unordered_map<int, std::function<void(uint8_t *)> *>> callbacks;
  std::thread connection_thread;
  static void manage_connection(std::string address,
                                std::shared_ptr<std::queue<OutboundMessage>> outbound_messages,
                                std::shared_ptr<std::unordered_map<int,
                                                                   std::function<void(uint8_t *)> *>> callbacks);
  static void receive_message(zmq::socket_t *socket,
                              std::shared_ptr<std::unordered_map<int,
                                                                 std::function<void(uint8_t *)> *>> callbacks);
  static void send_messages(zmq::socket_t *socket,
                            std::shared_ptr<std::queue<OutboundMessage>> outbound_messages,
                            std::shared_ptr<std::unordered_map<int,
                                                               std::function<void(uint8_t *)> *>> callbacks,
                            int *msg_id);
};

#endif //TEST_RPCCONNECTION_H
