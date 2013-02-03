
// This file is auto-generated from nullalgo.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_NULLALGO_CLIENT_HPP_
#define JUBATUS_CLIENT_NULLALGO_CLIENT_HPP_


#include "nullalgo_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class nullalgo : public pfi::network::mprpc::rpc_client {
public:
  nullalgo(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    bool update_random(std::string name, datum param) {
      return call<bool(std::string, datum)>("update_random")(name, param);
    }

    datum query_random(std::string name, std::string param) {
      return call<datum(std::string, std::string)>("query_random")(name, param);
    }

    bool update_cht(std::string name, std::string id, datum param) {
      return call<bool(std::string, std::string, datum)>("update_cht")(name, id, param);
    }

    datum query_cht(std::string name, std::string id, std::string param) {
      return call<datum(std::string, std::string, std::string)>("query_cht")(name, id, param);
    }

    bool save(std::string name, std::string id) {
      return call<bool(std::string, std::string)>("save")(name, id);
    }

    bool load(std::string name, std::string id) {
      return call<bool(std::string, std::string)>("load")(name, id);
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
      return call<std::map<std::string, std::map<std::string, std::string > >(std::string)>("get_status")(name);
    }

private:
};

} // namespace client

} // namespace jubatus



#endif // JUBATUS_CLIENT_NULLALGO_CLIENT_HPP_
