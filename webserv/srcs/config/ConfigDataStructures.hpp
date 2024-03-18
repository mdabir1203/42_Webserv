
#ifndef CONFIG_DATA_STRUCTURES_HPP

# define CONFIG_DATA_STRUCTURES_HPP

# include "webserv.hpp"

# define parseMap std::map<std::string, void (ConfigServer::*)(fileVector)>

class ConfigServer;
class RequestConfig;

typedef struct	s_listen {
	unsigned int	host;
	int			port;
}				t_listen;

typedef struct  s_error_page {
	std::vector<int>    errorCodes; // all of the codes that will be redirected
	std::string         uri;		// uri to which they are redirected
}               t_error_page;

#endif
