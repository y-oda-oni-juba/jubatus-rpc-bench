// -*- mode: c++; coding: utf-8-unix -*-

#ifndef __JUBATUS_RPC_BENCH_UTILS_H__
#define __JUBATUS_RPC_BENCH_UTILS_H__

#include <pficommon/system/time_util.h>

namespace jubatus {
namespace rpc_bench {

class TimeSpan {
public:
  typedef pfi::system::time::clock_time time_type;
  
  TimeSpan();
  void start();
  void stop();
  double elapsed_time_msec() const;

private:
  static time_type get_time();

  time_type start_;
  time_type end_;
};

int parse_positive_number(const char *optname, const char *optarg, int lower_bound = 1 );

} // rpc_bench
} // jubatus

#endif
