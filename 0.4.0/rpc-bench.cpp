// -*- mode: c++; coding: utf-8-unix -*-

#include <getopt.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include <pficommon/concurrent/thread.h>
#include <pficommon/lang/bind.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/system/time_util.h>

#include "exception.h"
#include "nullalgo_client.hpp"
#include "utils.h"

static const char *version_string = "1.0.0.130206a";

namespace jubatus {
namespace rpc_bench {

class Task {
public:
  enum {
    METHOD_QUERY_CHT,
    METHOD_QUERY_CHT_LARGE,
    METHOD_QUERY_CHT_NOLOCK,
  };

public:
  Task(int id, int cht_size, int query_num, int method_id, 
       const std::string &host, int port, std::string &name) :
    id_(id), query_num_(query_num), method_id_(method_id),
    host_(host), port_(port), name_(name),
    timeout_sec_( 60.0 ), dump_rpc_response_(false),
    has_error_(false) {

    for(int i = 0; i < cht_size; ++i ) {
      int cht_id = (i + id_) % cht_size;
      std::stringstream cht_id_ss;
      cht_id_ss << cht_id;
      cht_ids_.push_back( cht_id_ss.str());
    }
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
  void set_dump_rpc_response(bool sw) { dump_rpc_response_ = sw; }
  
private:
  void run() {
    has_error_ = false;
    latency_rec_.reserve(query_num_);

    std::string key_str( method_id_ == METHOD_QUERY_CHT_LARGE ? "large" : "" );

    jubatus::client::nullalgo client( host_, port_, timeout_sec_ );

    try {
      // examine
      exec_time_.start();
      for(int i = 0; i < query_num_; ++i ) {
        const std::string &cht_id = cht_ids_[ i % cht_ids_.size() ];

        TimeSpan lap_time;
        lap_time.start();
        if ( !dump_rpc_response_ ) {
          if ( method_id_ == METHOD_QUERY_CHT_NOLOCK )
            (void)client.query_cht_nolock( name_, cht_id, key_str);
          else {
            (void)client.query_cht( name_, cht_id, key_str);
          }
        } else {
          jubatus::datum response;
          if ( method_id_ == METHOD_QUERY_CHT_NOLOCK )
            response = client.query_cht_nolock( name_, cht_id, key_str);
          else {
            response = client.query_cht( name_, cht_id, key_str);
          }
          
          inspect_rpc_response(response, i, std::cerr);
        }
        lap_time.stop();

        latency_rec_.push_back( lap_time.elapsed_time_msec() );
      }
      exec_time_.stop();
    } catch ( const std::exception &e ) {
      std::cerr << "*** Exception caught ***: " << e.what() << std::endl;
      has_error_ = true;
    }
  }

  void inspect_rpc_response( const jubatus::datum &response, int nth, 
                             std::ostream &out = std::cerr ) {
    out << "task[" << id_ << "] " << (nth+1) << "/" << query_num_ << std::endl;

    const std::vector< std::pair<std::string, std::string> > &sv = response.string_values;
    if ( !sv.empty() ) {
      out << "  sv: " << sv.size() << " items" << std::endl;
      out << "    item size = {";
      for(size_t i = 0; i < sv.size(); ++i ) {
        out << " " << sv[i].second.size();
      }
      out << " }" << std::endl;
    }

    const std::vector< std::pair<std::string, double> > &nv = response.num_values;
    if ( !nv.empty() ) {
      out << "  nv: " << nv.size() << "items" << std::endl;
      out << "    items = {";
      for(size_t i = 0; i < nv.size(); ++i ) {
        out << " " << nv[i].second;
      }
      out << std::endl;
    }
  }

  int id_;
  int query_num_;
  int method_id_;
  std::string host_;
  int port_;
  std::string name_;
  double timeout_sec_;
  bool dump_rpc_response_;
  std::vector<std::string> cht_ids_;

  pfi::lang::shared_ptr<pfi::concurrent::thread> thread_;

  std::vector<double> latency_rec_;
  TimeSpan exec_time_;
  bool has_error_;
};

} // rpc_bench
} // jubatus

//// main

static int parse_method_id(const char *method_name) {
  using namespace jubatus::rpc_bench;

  if ( strcasecmp( method_name, "query_cht" ) == 0 )
    return Task::METHOD_QUERY_CHT;
  else if  ( strcasecmp( method_name, "query_cht_large" ) == 0 )
    return Task::METHOD_QUERY_CHT_LARGE;
  else if  ( strcasecmp( method_name, "query_cht_nolock" ) == 0 )
    return Task::METHOD_QUERY_CHT_NOLOCK;
  
  throw jubatus::rpc_bench::InvalidOption( "invalid method-type" );
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
      << "  --method METHOD ( query_cht, query_cht_large, query_cht_nolock )" << std::endl
      << "  --timeout SEC" << std::endl
      << "  --dump-latency" << std::endl
      << "  --dump-rpc-response" << std::endl
      << "  --no-divide-query" << std::endl
      << "  --cht-size-hint" << std::endl
      << "  --version" << std::endl
      << "  --help" << std::endl;
}

int main(int argc, char **argv) {
  std::string host = "localhost";
  int port = 9000;
  std::string cluster_name = "rpc-bench";
  int thread_num = 1;
  int query_num = 1000;
  int method_id = jubatus::rpc_bench::Task::METHOD_QUERY_CHT;
  int timeout_sec = 60;
  bool dump_latency = false;
  bool dump_rpc_response = false;
  bool divide_query = true;
  int cht_size_hint = 16;

  enum {
    OPTION_HOST = 100,
    OPTION_PORT,
    OPTION_NAME,
    OPTION_THREAD_NUM,
    OPTION_QUERY_NUM,
    OPTION_METHOD,
    OPTION_TIMEOUT,
    OPTION_DUMP_LATENCY,
    OPTION_DUMP_RPC_RESPONSE,
    OPTION_NO_DIVIDE_QUERY,
    OPTION_CHT_SIZE_HINT,

    OPTION_VERSION,
    OPTION_HELP,
  };
  struct option longopts[] = {
    { "host",           required_argument, NULL, OPTION_HOST },
    { "port",           required_argument, NULL, OPTION_PORT },
    { "name",           required_argument, NULL, OPTION_NAME },
    { "thread",         required_argument, NULL, OPTION_THREAD_NUM },
    { "query",          required_argument, NULL, OPTION_QUERY_NUM },
    { "method",         required_argument, NULL, OPTION_METHOD },
    { "timeout",        required_argument, NULL, OPTION_TIMEOUT },
    { "dump-latency",   no_argument,       NULL, OPTION_DUMP_LATENCY },
    { "dump-rpc-response", no_argument,    NULL, OPTION_DUMP_RPC_RESPONSE },
    { "no-divide-query",no_argument,       NULL, OPTION_NO_DIVIDE_QUERY },
    { "cht-size-hint",  required_argument, NULL, OPTION_CHT_SIZE_HINT },

    { "version",        no_argument,       NULL, OPTION_VERSION },
    { "help",           no_argument,       NULL, OPTION_HELP },

    { 0, 0, 0, 0 }
  };

  try {
    using jubatus::rpc_bench::parse_positive_number;

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
      case OPTION_METHOD:
        method_id = parse_method_id( optarg );
        break;
      case OPTION_TIMEOUT:
        timeout_sec = parse_positive_number( "timeout", optarg );
        break;
      case OPTION_DUMP_LATENCY:
        dump_latency = true;
        break;
      case OPTION_DUMP_RPC_RESPONSE:
        dump_rpc_response = true;
        break;
      case OPTION_NO_DIVIDE_QUERY:
        divide_query = false;
        break;
      case OPTION_CHT_SIZE_HINT:
        cht_size_hint = parse_positive_number( "cht-size-hint", optarg );
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

  int query_num_per_thr = divide_query ? (query_num + thread_num -1)/thread_num : query_num;

  for(int i = 0; i < thread_num; ++i ) {
    task_ptr task( new task_type(i, cht_size_hint, query_num_per_thr, method_id, 
                                 host, port, cluster_name ) );
    task->set_timeout_sec( double(timeout_sec) );
    task->set_dump_rpc_response( dump_rpc_response );
    tasks.push_back(task);
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
  int query_total_count = 0;
  for( int i = 0; i < thread_num; ++i ) {
    const std::vector<double>&latency_rec = tasks[i]->latency_records();
    for(int j = 0; j < (int)latency_rec.size(); ++j ) latency_total += latency_rec[j];
    query_total_count += latency_rec.size();
  }
  double latency = query_total_count == 0 ? NAN : latency_total/query_total_count;

  double query_per_sec = 
    ( total_exec_time.elapsed_time_msec() == 0 ?
      NAN :
      double(query_total_count)/(total_exec_time.elapsed_time_msec()/1000.0) );

  std::cout << "#thread: " << thread_num << std::endl;
  std::cout << "total query: " << query_total_count << std::endl;
  std::cout << "total time(msec): " << total_exec_time.elapsed_time_msec() << std::endl;
  std::cout << "query/sec: " << query_per_sec << std::endl;
  std::cout << "mean latency(msec): " << latency << std::endl;

  if ( dump_latency ) {
    std::cout << "latency(msec): ";
    bool first = true;
    for( int i = 0; i < thread_num; ++i ) {
      const std::vector<double>&latency_rec = tasks[i]->latency_records();
      for( int j = 0; j < (int)latency_rec.size(); ++j ) {
        if ( !first ) std::cout << ", ";
        std::cout << latency_rec[j];
        first = false;
      }
    }
    std::cout << std::endl;
  }

  return 0;
}
