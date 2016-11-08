#ifndef TEST_MOCKCONFIGSTORE_H
#define TEST_MOCKCONFIGSTORE_H

#include "ModelContainer.h"
class ModelConfigStore {
 public:
  void insert(int id, ModelContainer container_config);
  const ModelContainer* get(int id) const;
  bool remove(int id);
 private:
  std::unordered_map<int, ModelContainer> containers;
};

#endif //TEST_MOCKCONFIGSTORE_H
