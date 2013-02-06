#include <sstream>
#include <string>
#include <jubatus/framework/mixer/mixer_factory.hpp>
#include "nullalgo_serv.hpp"
#include <glog/logging.h>

using namespace jubatus::framework;

namespace jubatus { namespace server { // do not change

const int nullalgo_serv::string_size_ = 32;
const int nullalgo_serv::large_string_size_ = 1024 * 1024;
const int nullalgo_serv::number_num_ = 1;

nullalgo_serv::nullalgo_serv(const server_argv& a,
                             const jubatus::common::cshared_ptr<jubatus::common::lock_service>& zk)
  :server_base(a)
{
  mixer_.reset(jubatus::framework::mixer::create_mixer(a, zk));
  mixable_holder_.reset(new mixable_holder());
  mixer_->set_mixable_holder(mixable_holder_);

  create_stock_datum();
}

nullalgo_serv::~nullalgo_serv()
{}

bool nullalgo_serv::update_random(const datum &param) {
  return true;
}

datum nullalgo_serv::query_random(const std::string &param) {
  return get_stock_datum(param);
}

bool nullalgo_serv::update_cht(const std::string &id, const datum &param) {
  return true;
}

datum nullalgo_serv::query_cht(const std::string &id, const std::string &param) {
  return get_stock_datum(param);
}

datum nullalgo_serv::query_cht_nolock(const std::string &id, const std::string &param) {
  return get_stock_datum(param);
}

bool nullalgo_serv::save(const std::string &id) {
  return true;
}

bool nullalgo_serv::load(const std::string &id) {
  return true;
}

void nullalgo_serv::get_status(status_t& status) const {
}

bool nullalgo_serv::set_config(const std::string &config) {
  return true;
}

mixer::mixer* nullalgo_serv::get_mixer() const {
  return mixer_.get();
}

pfi::lang::shared_ptr<mixable_holder> nullalgo_serv::get_mixable_holder() const {
  return mixable_holder_;
}

void nullalgo_serv::create_stock_datum() {
  using namespace std;

  // string datum
  string_datum_.string_values.push_back( make_pair( string("s"), 
                                                    string( string_size_, 's')));

  // large string datum
  large_string_datum_.string_values.push_back( make_pair( string("l"),
                                                          string( large_string_size_, 'l')));

  // number datum
  for(int i = 0; i < number_num_; ++i ) {
    stringstream key_ss;
    key_ss << i;
    number_datum_.num_values.push_back( make_pair( key_ss.str(), double(i)));
  }
}

datum nullalgo_serv::get_stock_datum( const std::string &result_type ) {
  if ( result_type == "large" )
    return large_string_datum_;
  else if ( result_type == "number" )
    return number_datum_;
  else
    return string_datum_;
}

}} // namespace jubatus::server
