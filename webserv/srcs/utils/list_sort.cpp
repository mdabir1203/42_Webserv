#include "webserv.hpp"

bool compare_langs(const std::pair<std::string, float> first, const std::pair<std::string, float> second)
{
  return ( first.second > second.second );
}