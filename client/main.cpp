#include <iostream>
#include "RPCClient.h"

int main() {
    const int container_1_id = 1;
    const std::string container_1_ip = "127.0.0.1";
    const int container_1_port = 7643;
    const int container_2_id = 2;
    const std::string container_2_ip = "127.0.0.1";
    const int container_2_port = 7644;

    RPCClient rpc_client;
    rpc_client.connect(container_1_id, container_1_ip, container_1_port);
    rpc_client.connect(container_2_id, container_2_ip, container_2_port);

    uint8_t* msg = (uint8_t*) malloc(4 * sizeof(uint8_t));
    char const* msg_text = "cat";
    memcpy(msg, (void *) msg_text, 4);

    rpc_client.send_message(msg, 4, container_1_id);
    rpc_client.send_message(msg, 4, container_2_id);
    rpc_client.shutdown();

    return 2;
}