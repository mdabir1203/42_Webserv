#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "webserv.hpp"

# include "Config.hpp"
# include "Server.hpp"

// class Server;

class Cluster {
public:
	Cluster(void);
	Cluster(const Cluster & src);
	~Cluster(void);

	Cluster & operator=(const Cluster & src);

	void	config(std::string fileconf);
	int		setup(void);
	void	run(void);
	void	clean(void);
private:
	Config						_config;
	std::map<long, Server>		_servers;
	std::map<long, Server *>	_sockets;
	std::vector<int>			_ready;
	fd_set						_fd_set;
	unsigned int				_fd_size;
	long						_max_fd;
};

#endif
