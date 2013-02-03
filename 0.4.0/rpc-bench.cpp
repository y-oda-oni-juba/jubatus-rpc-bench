// -*- mode: c++; coding: utf-8-unix -*-

#include <getopt.h>
#include <time.h>
#include <unistd.h>

#include <exception>
#include <sstream>
#include <string>
#include <vector>

#include <pficommon/system/time_util.h>

#include "nullalgo_client.hpp"

std::string host = "localhost";
int port = 9000;
std::string cluster_name = "rpc-bench";
int thread_num = 1;
int query_num = 1000;
double timeout_sec = 5.0;

static const char *version_string = "1.0.0.130201a";

namespace jubatus {
namespace rpc_bench {

class InvalidOption: public std::runtime_error {
public:
  explicit InvalidOption( const std::string &what_mesg ) : std::runtime_error(what_mesg) {
  }
};

class TimeSpan {
public:
  typedef pfi::system::time::clock_time time_type;

  TimeSpan()
    : start_(0, 0), end_(0, 0) {
  }
  void start() { start_ = TimeSpan::getTime(); }
  void stop() { end_ = TimeSpan::getTime(); }
  int elapsedTimeMSec() const {
    time_type diff = end_ - start_;
    return diff.sec * 1000 + diff.usec/1000;
  }

private:
  static time_type getTime() {
    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );
    return time_type( ts.tv_sec, ts.tv_nsec/1000 );
  }

  time_type start_;
  time_type end_;
};

} // rpc_bench
} // jubatus

static int parse_positive_number(const char *optname, const char *optarg, 
                                 int lower_bound = 1 ) {
  int val = strtol(optarg, NULL, 10);
  if ( val < lower_bound ) {
    std::stringstream ss;
    ss << "--" << optname << " requires number >= " << lower_bound;
    throw jubatus::rpc_bench::InvalidOption( ss.str() );
  }

  return val;
}

static void show_version() {
  std::cout << "rpc-bench version " << version_string << std::endl;
}

static void show_usage( std::ostream &out = std::cerr ) {
  out << "Usage: rpc-bench [Options]" << std::endl
      << "Options:" << std::endl
      << "  --host HOSTNAME" << std::endl
      << "  --port PORT" << std::endl
      << "  --name CLUSTER_NAME" << std::endl
      << "  --thread #THREAD" << std::endl
      << "  --query #QUERY" << std::endl
      << "  --version" << std::endl
      << "  --help" << std::endl;
}

int main(int argc, char **argv) {
  enum {
    OPTION_HOST = 100,
    OPTION_PORT,
    OPTION_NAME,
    OPTION_THREAD_NUM,
    OPTION_QUERY_NUM,

    OPTION_VERSION,
    OPTION_HELP,
  };
  struct option longopts[] = {
    { "host",   required_argument, NULL, OPTION_HOST },
    { "port",   required_argument, NULL, OPTION_PORT },
    { "name",   required_argument, NULL, OPTION_NAME },
    { "thread", required_argument, NULL, OPTION_THREAD_NUM },
    { "query",  required_argument, NULL, OPTION_QUERY_NUM },

    { "version",no_argument,       NULL, OPTION_VERSION },
    { "help",   no_argument,       NULL, OPTION_HELP },

    { 0, 0, 0, 0 }
  };

  try {
    int opt = 0;
    while( (opt = getopt_long_only(argc, argv, "", longopts, NULL)) != -1 ) {
      switch(opt) {
      case OPTION_HOST:
        host = optarg;
        break;
      case OPTION_PORT:
        port = parse_positive_number( "port", optarg );
        break;
      case OPTION_NAME:
        cluster_name = optarg;
        break;
      case OPTION_THREAD_NUM:
        thread_num = parse_positive_number( "thread", optarg );
        break;
      case OPTION_QUERY_NUM:
        query_num = parse_positive_number( "query", optarg );
        break;
      case OPTION_VERSION:
        show_version();
        return 0;
      case OPTION_HELP:
        show_usage( std::cout );
        return 0;
      
      case '?':
        show_usage( std::cerr);
        return 1;
      }
    }
  } catch ( const jubatus::rpc_bench::InvalidOption &e ) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
    
  jubatus::client::nullalgo client( host, port, timeout_sec );

  try {
    std::cout << "start" << std::endl;
    
    // examine
    std::vector<int> latency_rec;
    latency_rec.reserve(query_num);

    jubatus::rpc_bench::TimeSpan ts;
    ts.start();
    for(int i = 0; i < query_num; ++i ) {
      jubatus::rpc_bench::TimeSpan lap_time;
      lap_time.start();
      jubatus::datum result = client.query_cht( cluster_name, "1", "key1");
      lap_time.stop();
      latency_rec.push_back( lap_time.elapsedTimeMSec() );
    }
    ts.stop();

    // statistics
    double latency_total = 0.0;
    for(int i = 0; i < query_num; ++i ) latency_total += latency_rec[i];
    double latency = latency_total/query_num;

    double query_per_sec = double(query_num)/(double(ts.elapsedTimeMSec())/1000.0);

    std::cout << "total time(msec): " << ts.elapsedTimeMSec() << std::endl;
    std::cout << "query/sec: " << query_per_sec << std::endl;
    std::cout << "latency(msec): " << latency << std::endl;

  } catch ( const std::exception &e ) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
  }

  return 0;
}
