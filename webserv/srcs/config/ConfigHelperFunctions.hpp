#ifndef CONFIG_HELPER_FUNCTIONS_HPP

# include "webserv.hpp"

# define CONFIG_HELPER_FUNCTIONS_HPP

bool 			isDigits(const std::string &str);
unsigned int	strToIp(std::string strIp);
std::string		removeAdjacentSlashes(const std::string &str);


#endif
