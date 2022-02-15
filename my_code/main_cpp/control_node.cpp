#include "iostream"
#include "control_node/control_impl.h"

using namespace std;

int main() {
    string control_meta_path = "control_meta";
    control_impl{control_meta_path, {
            {"127.0.0.1", 6666},
            {"127.0.0.1", 6667},
            {"127.0.0.1", 6668}}}.run();
}