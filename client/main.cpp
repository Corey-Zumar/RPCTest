#include <iostream>
#include <thread>
#include <zconf.h>
#include "RPCClient.h"
#include "RPCServer.h"
#include "RPCUtil.h"

void serve_model_discovery(ModelConfigStore* model_store) {
  RPCServer discover_server(model_store);
  // Read from conf file in future
  discover_server.start(RPCUtil::get_address("127.0.0.1", 7005));
}

void handle_response(uint8_t* msg, size_t len) {
  ((char*) msg)[len] = '\0';
  printf("%s\n", (char*) msg);
}

void test_functionality() {
  ModelConfigStore model_store;
  int model1_id = 1;
  ModelContainer model1 = ModelContainer(RPCUtil::get_address("127.0.0.1", 7010));
  model_store.insert(model1_id, model1);

  RPCClient client = RPCClient(&model_store);
  client.connect(model1_id, NULL);

  std::function<void(uint8_t*, size_t)> callback = handle_response;
  for(int i = 0; i < 10; i++) {
    usleep(1000000);
    client.send_message((uint8_t*) "cat", 4, model1_id, &callback);
    printf("SENT MESSAGE\n");
  }
  usleep(6000000);
  client.disconnect(model1_id);
  usleep(5000000);
}

void benchmark() {
  ModelConfigStore model_store;
  int model1_id = 1;
  ModelContainer model1 = ModelContainer(RPCUtil::get_address("127.0.0.1", 7010));
  model_store.insert(model1_id, model1);

  RPCClient client = RPCClient(&model_store);
  client.connect(model1_id, NULL);

  std::chrono::milliseconds start_ms = std::chrono::duration_cast<std::chrono::milliseconds >(
      std::chrono::system_clock::now().time_since_epoch());

  long start = 0;

  std::function<void(uint8_t*, size_t)> callback = [&](uint8_t* msg, size_t len) {
    std::chrono::milliseconds end_ms = std::chrono::duration_cast<std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch());
    printf("TIME TAKEN: %d ms\n", end_ms.count() - start);
  };

  uint8_t* data = (uint8_t*) malloc(8 * 784 * 500);
  start = start_ms.count();
  client.send_message(data, 8 * 784 * 500, model1_id, &callback);
  usleep(10000000);
}

int main() {
  benchmark();
  return 2;
}