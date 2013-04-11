// -*- mode: c++; coding: utf-8-unix -*-

#include <getopt.h>
#include <iostream>
#include <sstream>

#include <msgpack.hpp>
#include <jubatus/msgpack/rpc/server.h>
#include <jubatus/msgpack/rpc/client.h>

#include "exception.h"
#include "utils.h"
#include "nullalgo_types.hpp"

static const char *version_string = "1.0.0.130207a";

namespace jubatus {
namespace rpc_bench {

class BareServer: public msgpack::rpc::server::base {
public:
  BareServer(int port, int thread_num) :
    port_(port), thread_num_(thread_num) {

    create_stock_datum();
  }

  virtual void dispatch( msgpack::rpc::request req ) {
    try {
      std::string method;
      req.method().convert( &method );

      if ( method == "query_cht" ) {
        eval_query_cht( req );
        return;
      }

      if ( method == "query_cht_nolock" ) {
        eval_query_cht_nolock( req );
        return;
      }

      req.error( msgpack::rpc::NO_METHOD_ERROR );

    } catch( msgpack::type_error &e ) {
      req.error( msgpack::rpc::ARGUMENT_ERROR );

    } catch( std::exception &e ) {
      req.error( std::string( e.what() ));
    }
  }
  
  void start() {
    this->instance.listen( "0.0.0.0", port_ );
    this->instance.start( thread_num_ );

    std::cerr << "bare-pc-serv: started: "
              << "port=" << port_ << ", #thread=" << thread_num_ << std::endl;
  }

  void join() {
    this->instance.join();
  }

  void stop() {
    this->instance.close();
  }

private:
  void eval_query_cht( msgpack::rpc::request req ) {
    msgpack::type::tuple</*name*/ std::string, /*cht_id*/ std::string, /*param*/ std::string> params;
    req.params().convert( &params );
    req.result( query_cht( params.get<2>() ) );
  }

  void eval_query_cht_nolock( msgpack::rpc::request req ) {
    msgpack::type::tuple</*name*/ std::string, /*cht_id*/ std::string, /*param*/ std::string> params;
    req.params().convert( &params );
    req.result( query_cht_nolock( params.get<2>() ) );
  }

  jubatus::datum query_cht(const std::string &param) {
    return get_stock_datum( param );
  }
  jubatus::datum query_cht_nolock(const std::string &param) {
    return get_stock_datum( param );
  }

  void create_stock_datum() {
    using namespace std;

    // string datum
    string_datum_.string_values.push_back( make_pair( string("s"), 
                                                      string( string_size_, 's')));

    // large string datum
    large_string_datum_.string_values.push_back( make_pair( string("l"),
                                                            string( large_string_size_, 'l')));

    // huge string datum
    huge_string_datum_.string_values.push_back( make_pair( string("h"),
                                                           string( huge_string_size_, 'h')));

    // number datum
    for(int i = 0; i < number_num_; ++i ) {
      std::stringstream key_ss;
      key_ss << i;
      number_datum_.num_values.push_back( make_pair( key_ss.str(), double(i)));
    }
  }

  jubatus::datum get_stock_datum( const std::string &result_type ) {
    if ( result_type == "large" )
      return large_string_datum_;
    if ( result_type == "huge" )
      return huge_string_datum_;
    else if ( result_type == "number" )
      return number_datum_;
    else
      return string_datum_;
  }
  
  int port_;
  int thread_num_;

  jubatus::datum string_datum_;
  jubatus::datum large_string_datum_;
  jubatus::datum huge_string_datum_;
  jubatus::datum number_datum_;

  static const int string_size_ = 32;
  static const int large_string_size_ = 1024 * 1024;
  static const int huge_string_size_ = 1024 * 1024 * 128;
  static const int number_num_ = 1;
};

} // rpc_bench
} // jubatus

//// main

static void show_version() {
  std::cout << "bare-rpc-serv version " << version_string << std::endl;
}

static void show_usage( std::ostream &out = std::cerr ) {
  out << "Usage: bare-rpc-serv [Options]" << std::endl
      << "Options:" << std::endl
      << "  --port PORT" << std::endl
      << "  --thread #THREAD" << std::endl
      << "  --version" << std::endl
      << "  --help" << std::endl;
}

int main(int argc, char **argv) {
  int port = 9000;
  int thread_num = 2;
  
  enum {
    OPTION_PORT,
    OPTION_THREAD,

    OPTION_VERSION,
    OPTION_HELP,
  };
  struct option longopts[] = {
    { "port",		required_argument, NULL, OPTION_PORT },
    { "thread",         required_argument, NULL, OPTION_THREAD },

    { "version",	no_argument,       NULL, OPTION_VERSION },
    { "help",           no_argument,       NULL, OPTION_HELP },

    { 0, 0, 0, 0 }
  };

  try {
    using jubatus::rpc_bench::parse_positive_number;

    int opt = 0;
    while( (opt = getopt_long_only( argc, argv, "", longopts, NULL )) != -1 ) {
      switch(opt) {
      case OPTION_PORT:
        port = parse_positive_number( "port", optarg );
        break;
      case OPTION_THREAD:
        thread_num = parse_positive_number( "thread", optarg );
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
    
  } catch( const jubatus::rpc_bench::InvalidOption &e ) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  try {
    jubatus::rpc_bench::BareServer server( port, thread_num );
    server.start();
    server.join();

  } catch( std::exception &e ) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 2;
  }
  return 0;
}
