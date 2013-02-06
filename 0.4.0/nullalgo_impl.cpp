// this program is automatically generated by jenerator. do not edit.
#include <jubatus/framework.hpp>
#include "nullalgo_server.hpp"
#include "nullalgo_serv.hpp"
using namespace jubatus;
using namespace jubatus::framework;
namespace jubatus { namespace server {
class nullalgo_impl_ : public nullalgo<nullalgo_impl_>
{
public:
  nullalgo_impl_(const server_argv& a):
    nullalgo<nullalgo_impl_>(a.timeout),
    p_(new server_helper<nullalgo_serv>(a, true))
  {}

  bool update_random(std::string name, datum param) //update random
  { JWLOCK__(p_); return get_p()->update_random(param); }

  datum query_random(std::string name, std::string param) //analysis random
  { JRLOCK__(p_); return get_p()->query_random(param); }

  bool update_cht(std::string name, std::string id, datum param) //update cht(2)
  { JWLOCK__(p_); return get_p()->update_cht(id, param); }

  datum query_cht(std::string name, std::string id, std::string param) //analysis cht(2)
  { JRLOCK__(p_); return get_p()->query_cht(id, param); }

  datum query_cht_nolock(std::string name, std::string id, std::string param) //nolock cht(2)
  { NOLOCK__(p_); return get_p()->query_cht_nolock(id, param); }

  bool save(std::string name, std::string id) //update broadcast
  { JWLOCK__(p_); return get_p()->save(id); }

  bool load(std::string name, std::string id) //update broadcast
  { JWLOCK__(p_); return get_p()->load(id); }

  std::map<std::string,std::map<std::string,std::string > > get_status(std::string name) //analysis broadcast
  { JRLOCK__(p_); return p_->get_status(); }
  int run(){ return p_->start(*this); };
  common::cshared_ptr<nullalgo_serv> get_p(){ return p_->server(); };
private:
  common::cshared_ptr<server_helper<nullalgo_serv> > p_;
};
}} // namespace jubatus::server
int main(int args, char** argv){
  return
    jubatus::framework::run_server<jubatus::server::nullalgo_impl_>
       (args, argv, "nullalgo");
}
