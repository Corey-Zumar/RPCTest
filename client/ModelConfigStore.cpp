#include <unordered_map>
#include "ModelConfigStore.h"

/**
 * A mock of the model config store for RPC testing. NOT thread safe
 */
ModelConfigStore::ModelConfigStore() : containers(std::unordered_map<int, ModelContainer>()) {

}

void ModelConfigStore::insert(int id, ModelContainer container_config) {
  containers.emplace(id, container_config);
}

ModelContainer* ModelConfigStore::get(int id) {
  std::unordered_map<int, ModelContainer>::iterator container = containers.find(id);
  if(container == containers.end()) {
    return nullptr;
  }
  return &container->second;
}

bool ModelConfigStore::remove(int id) {
  std::unordered_map<int, ModelContainer>::const_iterator container = containers.find(id);
  if(container == containers.end()) {
    return false;
  }
  containers.erase(id);
  return true;
}