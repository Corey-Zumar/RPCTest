#include "RPCServer.h"
#include "RPCUtil.h"

RPCServer::RPCServer(ModelConfigStore *model_store)
    : server_socket(zmq::socket_t(RPCUtil::context, ZMQ_REP)) {
  models = model_store;
}

void RPCServer::start(std::string address) {
  printf("Hosting on %s\n", address.c_str());
  server_socket.bind(address);
  int curr_frame = 0;
  while (true) {
    zmq::message_t msg;
    server_socket.recv(&msg, 0);
    if (msg.size() <= 0) {
      continue;
    }
    // Process frame based on index
    switch (curr_frame) {
      case 0:
        save_container(&msg);
        send_ack();
        break;
      default:
        // Nothing
        break;
    }
    // End processing
    if (msg.more()) {
      curr_frame++;
    } else {
      curr_frame = 0;
    }
  }
}

void RPCServer::save_container(zmq::message_t* msg) {
  // In the future, we will deserialize the data and pass a message containing all
  // relevant attributes of a new model container to the model config store
  char *data = (char *) msg->data();
  std::string address = std::string(data);
  models->insert(next_container_id++, ModelContainer(address));
  printf("Discovered client: %s\n", address.c_str());
}

void RPCServer::send_ack() {
  uint8_t *buf = (uint8_t *) malloc(4 * sizeof(char));
  memcpy(buf, "ACK", 4);
  server_socket.send((void *) buf, 4, 0);
}
