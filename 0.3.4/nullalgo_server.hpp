
// This file is auto-generated from nullalgo.idl
// *** DO NOT EDIT ***

#ifndef NULLALGO_SERVER_HPP_
#define NULLALGO_SERVER_HPP_


#include "nullalgo_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>


namespace jubatus {

namespace server {

template <class Impl>
class nullalgo : public pfi::network::mprpc::rpc_server {
public:
  nullalgo(double timeout_sec): rpc_server(timeout_sec) {

    rpc_server::add<bool(std::string, datum) >("update_random", pfi::lang::bind(&Impl::update_random, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<datum(std::string, std::string) >("query_random", pfi::lang::bind(&Impl::query_random, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string, datum) >("update_cht", pfi::lang::bind(&Impl::update_cht, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<datum(std::string, std::string, std::string) >("query_cht", pfi::lang::bind(&Impl::query_cht, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<datum(std::string, std::string, std::string) >("query_cht_nolock", pfi::lang::bind(&Impl::query_cht_nolock, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2, pfi::lang::_3));
    rpc_server::add<bool(std::string, std::string) >("save", pfi::lang::bind(&Impl::save, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<bool(std::string, std::string) >("load", pfi::lang::bind(&Impl::load, static_cast<Impl*>(this), pfi::lang::_1, pfi::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string > >(std::string) >("get_status", pfi::lang::bind(&Impl::get_status, static_cast<Impl*>(this), pfi::lang::_1));
  }
};

} // namespace server

} // namespace jubatus



#endif // NULLALGO_SERVER_HPP_
