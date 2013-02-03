// -*- mode: c++; coding: utf-8-unix -*-

#include <getopt.h>
#include <time.h>
#include <unistd.h>

#include <exception>
#include <sstream>
#include <string>
#include <vector>

#include <pficommon/concurrent/thread.h>
#include <pficommon/lang/bind.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/system/time_util.h>

#include "nullalgo_client.hpp"

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
  void start() { start_ = TimeSpan::get_time(); }
  void stop() { end_ = TimeSpan::get_time(); }
  double elapsed_time_msec() const {
    time_type diff = end_ - start_;
    return diff.sec * 1000 + double(diff.usec)/1000.0;
  }

private:
  static time_type get_time() {
    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );
    return time_type( ts.tv_sec, ts.tv_nsec/1000 );
  }

  time_type start_;
  time_type end_;
};

class Task {
public:
  Task(int id, int query_num, const std::string &host, int port, std::string &name) :
    id_(id), query_num_(query_num), host_(host), port_(port), name_(name),
    timeout_sec_( 5.0 ),
    has_error_(false) {
  }

  void start() {
    using namespace pfi::lang;
    using namespace pfi::concurrent;
    
    thread_.reset( new thread( bind( &Task::run, this ) ) );
    thread_->start();
  }

  void wait() {
    thread_->join();
  }

  const TimeSpan &exec_time() { return exec_time_; }
  const std::vector<double> &latency_records() { return latency_rec_; }
  bool has_error() const;
  void set_timeout_sec( double timeout_sec ) { timeout_sec_ = timeout_sec; }
  
private:
  void run() {
    has_error_ = false;
    latency_rec_.reserve(query_num_);

    std::stringstream id_ss;
    id_ss << id_;
    std::string id_str( id_ss.str() );

    std::string key_str( "" );

    jubatus::client::nullalgo client( host_, port_, timeout_sec_ );

    try {
      // examine
      exec_time_.start();
      for(int i = 0; i < query_num_; ++i ) {
        TimeSpan lap_time;
        lap_time.start();
        (void)client.query_cht( name_, id_str, key_str);
        lap_time.stop();

        latency_rec_.push_back( lap_time.elapsed_time_msec() );
      }
      exec_time_.stop();
    } catch ( const std::exception &e ) {
      std::cerr << "Exception caught: " << e.what() << std::endl;
      has_error_ = true;
    }
  }

  int id_;
  int query_num_;
  std::string host_;
  int port_;
  std::string name_;
  double timeout_sec_;

  pfi::lang::shared_ptr<pfi::concurrent::thread> thread_;

  std::vector<double> latency_rec_;
  TimeSpan exec_time_;
  bool has_error_;
};

} // rpc_bench
} // jubatus

//// main

std::string host = "localhost";
int port = 9000;
std::string cluster_name = "rpc-bench";
int thread_num = 1;
int query_num = 1000;

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

  typedef jubatus::rpc_bench::Task task_type;
  typedef pfi::lang::shared_ptr<task_type> task_ptr;
  std::vector<task_ptr> tasks;

  for(int i = 0; i < thread_num; ++i ) {
    tasks.push_back( task_ptr( new task_type(i, query_num, host, port, cluster_name ) ) );
  }

  jubatus::rpc_bench::TimeSpan total_exec_time;
  total_exec_time.start();
  for(int i = 0; i < thread_num; ++i ) {
    tasks[i]->start();
  }
  for(int i = 0; i < thread_num; ++i ) {
    tasks[i]->wait();
  }
  total_exec_time.stop();

  // statistics
  double latency_total = 0.0;
  for( int i = 0; i < thread_num; ++i ) {
    const std::vector<double>&latency_rec = tasks[i]->latency_records();
    for(int i = 0; i < query_num; ++i ) latency_total += latency_rec[i];
  }
  double latency = latency_total/(query_num*thread_num);

  double query_per_sec = double(query_num*thread_num)/(total_exec_time.elapsed_time_msec()/1000.0);

  std::cout << "#thread: " << thread_num << std::endl;
  std::cout << "total query: " << query_num*thread_num << std::endl;
  std::cout << "total time(msec): " << total_exec_time.elapsed_time_msec() << std::endl;
  std::cout << "query/sec: " << query_per_sec << std::endl;
  std::cout << "latency(msec): " << latency << std::endl;

  return 0;
}
