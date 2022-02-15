#include "iostream"
#include "store_node/store_impl.h"

using namespace std;

int main(int argc, char **argv) {
    int port = (int)strtoll(argv[1], nullptr, 10);
    store_impl{"data" + to_string(port), "log" + to_string(port), port}.run();
}