
#include "nullalgo_serv.hpp"
#include <glog/logging.h>

using namespace jubatus::framework;

namespace jubatus { namespace server { // do not change

nullalgo_serv::nullalgo_serv(const server_argv& a,
                             const jubatus::common::cshared_ptr<jubatus::common::lock_service>& zk)
  :server_base(a)
{
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
  return number_datum_;
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
  return NULL;
}

pfi::lang::shared_ptr<mixable_holder> nullalgo_serv::get_mixable_holder() const {
  return pfi::lang::shared_ptr<mixable_holder>();
}

void nullalgo_serv::create_stock_datum() {
  // string datum
  string_datum_.string_values.push_back( std::make_pair( std::string("key1"), std::string("value1")));
  string_datum_.string_values.push_back( std::make_pair( std::string("key2"), std::string("value2")));
  string_datum_.string_values.push_back( std::make_pair( std::string("key3"), std::string("value3")));
  
  // number datum
  number_datum_.num_values.push_back( std::make_pair( std::string("key1"), 1.0));
  number_datum_.num_values.push_back( std::make_pair( std::string("key2"), 2.0));
  number_datum_.num_values.push_back( std::make_pair( std::string("key3"), 3.0));
}

}} // namespace jubatus::server
