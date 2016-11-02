#ifndef TEST_MOCKCONFIGSTORE_H
#define TEST_MOCKCONFIGSTORE_H

#include "ModelContainer.h"
class ModelConfigStore {
 public:
  ModelConfigStore();
  void insert(int id, ModelContainer container_config);
  ModelContainer* get(int id);
  bool remove(int id);
 private:
  std::unordered_map<int, ModelContainer> containers;
};

#endif //TEST_MOCKCONFIGSTORE_H
