#include <thread>
#include "RPCServer.h"
#include "RPCUtil.h"

using namespace std;
using namespace zmq;

RPCServer::RPCServer(ModelConfigStore *model_store, function<void(int)> *callback) {
  models = model_store;
  connection_callback = callback;
}

RPCServer::~RPCServer() {
  stop();
}

void RPCServer::start(string address) {
  server_thread = thread(&RPCServer::listen, address, models, move(connection_callback), &interrupted);
  server_thread.detach();
}

void RPCServer::stop() {
  interrupted = true;
}

bool RPCServer::save_container(int container_id, message_t *msg, ModelConfigStore *models) {
  // In the future, we will deserialize the data and pass a message containing all
  // relevant attributes of a new model container to the model config store
  char *data = (char *) msg->data();
  string address = string(data);
  models->insert(container_id, ModelContainer(address));
  printf("Discovered client: %s\n", address.c_str());
  return true;
}

void RPCServer::send_ack(socket_t *socket) {
  uint8_t *buf = (uint8_t *) malloc(4 * sizeof(char));
  memcpy(buf, "ACK", 4);
  socket->send((void *) buf, 4, 0);
}

void RPCServer::listen(string address,
                       ModelConfigStore *models,
                       function<void(int)> *connection_callback,
                       bool *interrupted) {
  printf("Hosting on %s\n", address.c_str());
  socket_t server_socket(RPCUtil::context, ZMQ_REP);
  server_socket.bind(address);
  int curr_frame = 0;
  int container_id = 0;
  while (true) {
    if (*interrupted) {
      shutdown(address, &server_socket);
      return;
    }
    message_t msg;
    server_socket.recv(&msg, 0);
    if (msg.size() <= 0) {
      continue;
    }
    // Process frame based on index
    switch (curr_frame) {
      case 0:
        if (save_container(container_id, &msg, models)) {
          send_ack(&server_socket);
          if(connection_callback) {
            connection_callback->operator()(container_id);
          }
          container_id = container_id + 1;
        } else {
          // Some sort of error handling
        }
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

void RPCServer::shutdown(string address, socket_t *socket) {
  socket->disconnect(address);
  socket->close();
}