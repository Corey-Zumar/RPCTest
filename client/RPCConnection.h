#ifndef TEST_RPCCONNECTION_H
#define TEST_RPCCONNECTION_H

#include <thread>
#include <string>
#include <queue>
#include <unordered_map>

class RPCConnection {
 public:
  RPCConnection(std::string address);
  ~RPCConnection();
  void queue_message(uint8_t *msg_content, size_t msg_len, std::function<void(uint8_t *, size_t)> *msg_callback);
  void shutdown();

 private:
  class OutboundMessage {
   public:
    OutboundMessage(uint8_t *msg_content, size_t msg_len, std::function<void(uint8_t *, size_t)> *msg_callback) {
      content = msg_content;
      len = msg_len;
      callback = msg_callback;
    }
    uint8_t *content;
    size_t len;
    std::function<void(uint8_t *, size_t)> *callback;
  };
  std::shared_ptr<std::queue<OutboundMessage>> outbound_queue;
  std::shared_ptr<std::unordered_map<std::string, std::function<void(uint8_t *, size_t)> *>> callbacks;
  std::shared_ptr<std::mutex> queue_lock;
  std::thread connection_thread;
  bool interrupted = false;
  static void manage_connection(std::string address,
                                bool* interrupted,
                                std::shared_ptr<std::mutex> queue_lock,
                                std::shared_ptr<std::queue<OutboundMessage>> outbound_messages,
                                std::shared_ptr<std::unordered_map<std::string,
                                                                   std::function<void(uint8_t *,
                                                                                      size_t)> *>> callbacks);
  static void receive_message(zmq::socket_t *socket,
                              std::shared_ptr<std::unordered_map<std::string,
                                                                 std::function<void(uint8_t *,
                                                                                    size_t)> *>> callbacks);
  static void send_messages(zmq::socket_t *socket,
                            std::shared_ptr<std::mutex> queue_lock,
                            std::shared_ptr<std::queue<OutboundMessage>> outbound_messages,
                            std::shared_ptr<std::unordered_map<std::string,
                                                               std::function<void(uint8_t *, size_t)> *>> callbacks,
                            int *msg_id);
  static void stop(zmq::socket_t *socket, std::string address);
};

#endif //TEST_RPCCONNECTION_H
