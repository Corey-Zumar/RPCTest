#ifndef TEST_MODELCONTAINER_H
#define TEST_MODELCONTAINER_H

#include <string>
#include <zmq.hpp>
class ModelContainer {
 public:
  ModelContainer(std::string container_address);
  std::string address;
};

#endif //TEST_MODELCONTAINER_H
