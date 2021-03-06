// this program is automatically generated by jenerator. do not edit.
#include <jubatus/framework.hpp>
#include <jubatus/framework/aggregators.hpp>
#include <jubatus/common/exception.hpp>
#include "nullalgo_types.hpp"
#include <glog/logging.h>
using namespace jubatus;
using namespace jubatus::framework;
int main(int args, char** argv){
  try{
    keeper k(keeper_argv(args,argv,"nullalgo"));
    k.register_random<bool, datum >("update_random"); //pass update
    k.register_random<datum, std::string >("query_random"); //pass analysis
    k.register_cht<2, bool, datum >("update_cht", pfi::lang::function<bool(bool,bool)>(&all_and)); //update
    k.register_cht<2, datum, std::string >("query_cht", pfi::lang::function<datum(datum,datum)>(&pass<datum >)); //analysis
    k.register_cht<2, datum, std::string >("query_cht_nolock", pfi::lang::function<datum(datum,datum)>(&pass<datum >)); //nolock
    k.register_broadcast<bool, std::string >("save", pfi::lang::function<bool(bool,bool)>(&all_and)); //update
    k.register_broadcast<bool, std::string >("load", pfi::lang::function<bool(bool,bool)>(&all_and)); //update
    k.register_broadcast<std::map<std::string,std::map<std::string,std::string > > >("get_status", pfi::lang::function<std::map<std::string,std::map<std::string,std::string > >(std::map<std::string,std::map<std::string,std::string > >,std::map<std::string,std::map<std::string,std::string > >)>(&merge<std::string,std::map<std::string,std::string > >)); //analysis
    return k.run();
  } catch (const jubatus::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}
