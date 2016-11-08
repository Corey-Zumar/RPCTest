#include <iostream>
#include <thread>
#include <zconf.h>
#include "RPCClient.h"
#include "RPCServer.h"
#include "RPCUtil.h"

using namespace std;

void handle_response(uint8_t* msg, size_t len) {
  ((char*) msg)[len] = '\0';
  printf("%s\n", (char*) msg);
}

void test_discovery(ModelConfigStore& model_store) {
  RPCClient client(model_store);

  function<void(uint8_t*, size_t)> msg_callback = handle_response;

  function<void(int)> callback = [&](int container_id) {
    printf("DISCOVERED CONTAINER: %d\n", container_id);
    client.connect(container_id, NULL);
    client.send_message((uint8_t*) "cat", 4, container_id, msg_callback);
  };

  RPCServer discover_server(model_store, callback);
  // Read from conf file in future
  discover_server.start(RPCUtil::get_address("127.0.0.1", 7005));
  for(int i = 0; i < 10; i++) {
    usleep(1000000);
  }
  discover_server.stop();
  usleep(2000000);
}

void test_functionality() {
  ModelConfigStore model_store;
  int model1_id = 1;
  ModelContainer model1 = ModelContainer(RPCUtil::get_address("127.0.0.1", 7010));
  model_store.insert(model1_id, model1);

  RPCClient client(model_store);
  client.connect(model1_id, NULL);

  std::function<void(uint8_t*, size_t)> callback = handle_response;
  for(int i = 0; i < 10; i++) {
    usleep(1000000);
    client.send_message((uint8_t*) "cat", 4, model1_id, callback);
    printf("SENT MESSAGE\n");
  }
  usleep(6000000);
  client.disconnect(model1_id);
  usleep(5000000);
}

void benchmark() {
  ModelConfigStore model_store;
  int model1_id = 1;
  ModelContainer model1(RPCUtil::get_address("127.0.0.1", 7010));
  model_store.insert(model1_id, model1);

  RPCClient client(model_store);
  client.connect(model1_id, [](bool successful) {

  });

  usleep(1000000);

  long total_millis = 0;

  for(int i = 0; i < 500; i++) {

    long start = 0;

    std::function<void(uint8_t *, size_t)> callback = [&](uint8_t *msg, size_t len) {
      std::chrono::milliseconds end_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch());
      printf("TIME TAKEN: %d ms\n", end_ms.count() - start);
      total_millis += (end_ms.count() - start);
    };

    uint8_t *data = (uint8_t *) malloc(8 * 784 * 500);
    start = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    string timestamp = "START: " + to_string(start) + "\n";
    printf(timestamp.c_str());
    client.send_message(data, 8 * 784 * 500, model1_id, callback);
    usleep(100000);
  }
  printf("AVG: %f\n", float(total_millis) / 500);
  usleep(10000000);
}

int main() {
  //ModelConfigStore store;
  //test_discovery(store);
  //test_functionality();
  benchmark();
  return 2;
}