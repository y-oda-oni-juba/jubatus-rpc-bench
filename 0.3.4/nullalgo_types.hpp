
// This file is auto-generated from nullalgo.idl
// *** DO NOT EDIT ***

#ifndef NULLALGO_TYPES_HPP_
#define NULLALGO_TYPES_HPP_


#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <msgpack.hpp>


namespace jubatus {

struct datum {
public:

  
  MSGPACK_DEFINE(string_values, num_values);  

  std::vector<std::pair<std::string, std::string > > string_values;
  std::vector<std::pair<std::string, double > > num_values;
};

} // namespace jubatus



#endif // NULLALGO_TYPES_HPP_
