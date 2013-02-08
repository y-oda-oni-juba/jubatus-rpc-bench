// -*- mode: c++; coding: utf-8-unix -*-

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>
#include <pficommon/system/time_util.h>
#include "exception.h"
#include "utils.h"

namespace jubatus {
namespace rpc_bench {

TimeSpan::TimeSpan()
  : start_(0, 0), end_(0, 0) {
}

void TimeSpan::start() {
  start_ = TimeSpan::get_time(); 
}

void TimeSpan::stop() {
  end_ = TimeSpan::get_time();
}

double TimeSpan::elapsed_time_msec() const {
  time_type diff = end_ - start_;
  return diff.sec * 1000 + double(diff.usec)/1000.0;
}

TimeSpan::time_type TimeSpan::get_time() {
  struct timespec ts;
  clock_gettime( CLOCK_REALTIME, &ts );
  return time_type( ts.tv_sec, ts.tv_nsec/1000 );
}

int parse_positive_number(const char *optname, const char *optarg, int lower_bound ) {
  int val = strtol(optarg, NULL, 10);
  if ( val < lower_bound ) {
    std::stringstream ss;
    ss << "--" << optname << " requires number >= " << lower_bound;
    throw jubatus::rpc_bench::InvalidOption( ss.str() );
  }
  
  return val;
}

} // rpc_bench
} // jubatus
