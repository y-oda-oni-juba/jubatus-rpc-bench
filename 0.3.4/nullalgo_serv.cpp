#include <sstream>
#include <string>
#include <jubatus/framework/mixer/mixer_factory.hpp>
#include "nullalgo_serv.hpp"
#include <glog/logging.h>

using namespace jubatus::framework;

namespace jubatus { namespace server { // do not change

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
  return string_datum_;
}

bool nullalgo_serv::update_cht(const std::string &id, const datum &param) {
  return true;
}

datum nullalgo_serv::query_cht(const std::string &id, const std::string &param) {
  return string_datum_;
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
  // string datum
  std::stringstream ss;
  for( int i = 0; i < 1024; ++i ) ss << '.';
  std::string value(ss.str());

  for(int i = 0; i < 1024; ++i ) {
    std::stringstream kss;
    kss << "key-" << i;
    string_datum_.string_values.push_back( std::make_pair( kss.str(), value ));
  }
  
  // number datum
  number_datum_.num_values.push_back( std::make_pair( std::string("key1"), 1.0));
  number_datum_.num_values.push_back( std::make_pair( std::string("key2"), 2.0));
  number_datum_.num_values.push_back( std::make_pair( std::string("key3"), 3.0));
}

}} // namespace jubatus::server
