// -*- mode: c++; coding: utf-8-unix -*-

#ifndef __JUBATUS_RPC_BENCH_EXCEPTION_H__
#define __JUBATUS_RPC_BENCH_EXCEPTION_H__

#include <string>
#include <stdexcept>

namespace jubatus {
namespace rpc_bench {

class InvalidOption: public std::runtime_error {
public:
  explicit InvalidOption( const std::string &what_mesg ) : std::runtime_error(what_mesg) {
  }
};

} // rpc_bench
} // jubatus

#endif
