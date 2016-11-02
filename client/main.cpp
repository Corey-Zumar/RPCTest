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

void handle_response(uint8_t* msg) {
  printf("%s\n", (char*) msg);
}

int main() {
  ModelConfigStore model_store;
  int model1_id = 1;
  ModelContainer model1 = ModelContainer(RPCUtil::get_address("127.0.0.1", 7010));
  model_store.insert(model1_id, model1);

  RPCClient client = RPCClient(&model_store);
  client.connect(model1_id, NULL);

  std::function<void(uint8_t*)> callback = handle_response;

  for(int i = 0; i < 10; i++) {
    usleep(1000000);
    client.send_message((uint8_t*) "cat", 4, model1_id, &callback);
    printf("SENT MESSAGE\n");
  }

  return 2;
}