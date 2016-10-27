#ifndef TEST_MODELCONTAINER_H
#define TEST_MODELCONTAINER_H

#include <string>
#include <zmq.hpp>
class ModelContainer {
 public:
  ModelContainer(std::string container_address, zmq::socket_t* container_socket);
  zmq::socket_t* socket;
  std::string address;
};

#endif //TEST_MODELCONTAINER_H
