// -*- mode: c++; coding: utf-8-unix -*-

#include <unistd.h>
#include <string>
#include "nullalgo_client.hpp"

std::string host = "localhost";
int port = 9000;
std::string cluster_name = "rpc-bench";
double timeout_sec = 5.0;

int main(int argc, char **argv) {
  jubatus::client::nullalgo client( host, port, timeout_sec );

  try {
    jubatus::datum result = client.query_cht( cluster_name, "1", "key1");
    std::cout << "#item=" << result.string_values.size() << std::endl;
    for( size_t i = 0; i < result.string_values.size(); ++i ) {
      std::cout << i << ": " 
                << result.string_values[i].first << " => " << result.string_values[i].second
                << std::endl;
    }
  } catch ( const std::exception &e ) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
  }

  return 0;
}
