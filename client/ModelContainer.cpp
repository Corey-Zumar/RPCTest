#include <sys/socket.h>
#include "ModelContainer.h"

ModelContainer::ModelContainer(std::string container_address, zmq::socket_t* container_socket) {
  socket = container_socket;
  address = container_address;
}