// -*- mode: c++; coding: utf-8-unix -*-

#ifndef JUBATUS_RPC_BENCH_NULLALGO_SERV_HPP_
#define JUBATUS_RPC_BENCH_NULLALGO_SERV_HPP_

#include <jubatus/framework.hpp>
#include "nullalgo_types.hpp"
using namespace jubatus::framework;

namespace jubatus { namespace server { // do not change
class nullalgo_serv : public server_base // do not change
{
public:
  nullalgo_serv(const server_argv& a,
           const jubatus::common::cshared_ptr<jubatus::common::lock_service>& zk); // do not change
  virtual ~nullalgo_serv(); // do not change

  bool update_random(const datum &param);
  datum query_random(const std::string &param);

  bool update_cht(const std::string &id, const datum &param);
  datum query_cht(const std::string &id, const std::string &param);

  // required for jubatus serv
  bool save(const std::string &id);
  bool load(const std::string &id);
  void get_status(status_t& status) const;
  bool set_config(const std::string &config);
  mixer::mixer* get_mixer() const;
  pfi::lang::shared_ptr<mixable_holder> get_mixable_holder() const;

private:
  void create_stock_datum();

  datum string_datum_;
  datum number_datum_;

  pfi::lang::scoped_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;
};
}} // namespace jubatus::server

#endif
