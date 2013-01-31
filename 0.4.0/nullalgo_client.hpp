
// This file is auto-generated from nullalgo.idl
// *** DO NOT EDIT ***

#ifndef NULLALGO_CLIENT_HPP_
#define NULLALGO_CLIENT_HPP_


#include "nullalgo_types.hpp"
#include <jubatus/msgpack/rpc/client.h>


namespace jubatus {

namespace client {

class nullalgo {
public:
  nullalgo(const std::string &host, uint64_t port, double timeout_sec)
    : c_(host, port) {
    c_.set_timeout( timeout_sec );
  }

    bool update_random(std::string name, datum param) {
      return c_.call("update_random", name, param).get<bool >();
    }

    datum query_random(std::string name, std::string param) {
      return c_.call("query_random", name, param).get<datum >();
    }

    bool update_cht(std::string name, std::string id, datum param) {
      return c_.call("update_cht", name, id, param).get<bool >();
    }

    datum query_cht(std::string name, std::string id, std::string param) {
      return c_.call("query_cht", name, id, param).get<datum >();
    }

    bool save(std::string name, std::string id) {
      return c_.call("save", name, id).get<bool >();
    }

    bool load(std::string name, std::string id) {
      return c_.call("load", name, id).get<bool >();
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
      return c_.call("get_status", name).get<std::map<std::string, std::map<std::string, std::string > > >();
    }

private:
  msgpack::rpc::client c_;
  msgpack::rpc::client c_;
  msgpack::rpc::client c_;
};

} // namespace client

} // namespace jubatus



#endif // NULLALGO_CLIENT_HPP_
