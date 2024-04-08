#include "webserv.hpp"

//It iterates through locations to find a match
//	- Checks deny,
//	then allow conditions on matching locations
//	-		Returns allow or configured deny code
//	-		No match denies the request

bool	ServerSocket::isMethodAllowed(const std::string &method, int locationIndex)
{
	for (std::map<std::string, std::string>::iterator it = currentServ.getLocationBegin(locationIndex); it != currentServ.getLocationEnd(locationIndex); it++)
	{
		if (it->second == "allow" && it->first == method)
		{
			return true;
		}
	}
	return false;
}

int ServerSocket::getReturnCode(int locationIndex)
{
	std::map<std::string, std::string>::iterator it = currentServ.getServLocation(locationIndex, "return");
	if (it != currentServ.getLocationEnd(locationIndex))
	{
		std::istringstream iss(it->second);
		double value;
		iss >> value;
		return value;
	}
	return 0;
}

bool ServerSocket::isLocationDenied(int locationIndex)
{
	std::map<std::string, std::string>::iterator it = currentServ.getServLocation(locationIndex, "deny");
	return it != currentServ.getLocationEnd(locationIndex) && it->second == "all";
}

#include <sys/stat.h> // Add the missing include statement

bool ServerSocket::doesLocationMatch(const std::string &path, int locationIndex)
{
	std::map<std::string, std::string>::iterator it = currentServ.getServLocation(locationIndex, "location");
	return it->second.substr(0, it->second.rfind("/")) == path.substr(0, path.rfind("/"));
}

int ServerSocket::checkPerms(const std::string &buffer)
{
	std::istringstream request(buffer);
	std::string method, path;
	request >> method >> path;

	std::string cpy_path = currentServ.getServConf("web_root")->second + path;
	struct stat s;
	if (stat(cpy_path.c_str(), &s) == 0 && s.st_mode & S_IFDIR && cpy_path[cpy_path.length() - 1] != '/')
		cpy_path.append("/");
	int i = 0;
	while (i < currentServ.getLocationSize())
	{
		if (doesLocationMatch(cpy_path, i)) {
			if (isLocationDenied(i)) {
				return getReturnCode(i);
			}
			if (isMethodAllowed(method, i)) {
				return 1;
			}
		}
		i++;
	}
}

std::map<int, std::string> ServerSocket::checkForRedirects(std::string path)
{
	std::map<int, std::string> response;
	std::map<std::string, std::string>::iterator it;

	for (int k = 0; k < currentServ.getLocationSize(); k++)
	{
		it = currentServ.getServLocation(k, "location");
		if (it != currentServ.getLocationEnd(k))
		{
			if (it->second == path)
			{
				it = currentServ.getServLocation(k, "redirect");
				if (it != currentServ.getLocationEnd(k))
				{
					response[1] = "HTTP/1.1 302 Found\r\nLocation: " + it->second + "\r\n\r\n";
					return response;
				}
			}
		}
	}

	return response;
}

void ServerSocket::normalizePath(std::string &path)
{
	if (path[path.length() - 1] != '/')
		path.append("/");
}

void ServerSocket::prependWebRoot(std::string &path)
{
	std::map<std::string, std::string>::iterator it = currentServ.getServConf("web_root");
	if (it != currentServ.getConfEnd())
		path = it->second + path;
}

void ServerSocket::appendDefaultFileIfRoot(std::string &path, int &trigger)
{
	if (path.length() == 1)
	{
		for (int i = 0; i < currentServ.getLocationSize(); i++)
		{
			std::map<std::string, std::string>::iterator it = currentServ.getServLocation(i, "location");
			if (it != currentServ.getLocationEnd(i))
			{
				if (it->second == "/")
				{
					it = currentServ.getServLocation(i, "default_file");
					if (it != currentServ.getLocationEnd(i))
					{
						path = path + it->second;
						trigger = 1;
					}
				}
			}
		}
	}
}

void ServerSocket::handleDirectoryOrFile(std::string &path, int &trigger)
{
	struct stat s;
	std::string path_cpy = path;
	std::map<std::string, std::string>::iterator it;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			// If path is a directory, check if autoindex is on
			for (int i = 0; i < currentServ.getLocationSize(); i++)
			{
				it = currentServ.getServLocation(i, "location");
				if (it != currentServ.getLocationEnd(i))
				{
					if (it->second == path)
					{
						it = currentServ.getServLocation(i, "autoindex");
						if (it != currentServ.getLocationEnd(i) && it->second == "on")
						{
							path = path + "index.html";
							trigger = 1;
						}
					}
				}
			}
		}
		else if (s.st_mode & S_IFREG)
		{
			// If path is a file, set trigger to 1
			trigger = 1;
		}
	}
}

std::map<int, std::string> ServerSocket::generateResponse(std::string path, int trigger)
{
	std::map<int, std::string> response;
	FILE *fin;

	if (trigger == 0)
	{
		// If trigger is 0, return 403 Forbidden
		response[1] = "HTTP/1.1 403 Forbidden\r\n\r\n";
	}
	else
	{
		// If trigger is 1, try to open the file
		fin = fopen(path.c_str(), "rb");
		if (fin == NULL)
		{
			// If file cannot be opened, return 404 Not Found
			response[1] = "HTTP/1.1 404 Not Found\r\n\r\n";
		}
		else
		{
			// If file can be opened, return 200 OK and the file content
			char buffer[1024];
			std::string fileContent;
			while (fgets(buffer, sizeof(buffer), fin) != NULL)
			{
				fileContent += buffer;
			}
			fclose(fin);

			response[0] = "HTTP/1.1 200 OK\r\n\r\n" + fileContent;
		}
	}

	return response;
}

std::map<int, std::string> ServerSocket::parseFileInfo(std::string path)
{
	int trigger = 0;

	std::map<int, std::string> response = checkForRedirects(path);
	if (!response.empty())
		return response;

	normalizePath(path);
	prependWebRoot(path);
	appendDefaultFileIfRoot(path, trigger);
	handleDirectoryOrFile(path, trigger);

	return generateResponse(path, trigger);
}

void ServerSocket::parseLocation(const std::vector<std::string> &tmpLine, int index, int ind_serv)
{
	(void) index;
	std::map<std::string, std::string> tmp;
	for (size_t i = 0; i < tmpLine.size(); i++)
	{
		std::string toRead = tmpLine[i];
		std::istringstream iss(toRead);
		std::string key, value;
		if (iss >> key)
		{
			if (iss >> value)
			{
				if (key == "return")
				{
					if (!checkValue(value))
					{
						std::cerr << "Error: 1 wrong config format - code error" << std::endl;
						exit(1);
					}
				}
				if (key == "}")
					break;
				if (key == "allow")
					tmp[value] = key;
				else
					tmp[key] = value;
			}
			else
			{
				std::cerr << "Error in location conf" << std::endl;
				exit(1);
			}
			if (iss >> value && i != 0)
			{
				std::cerr << "Error: too many arguments in one line in conf" << std::endl;
				exit(1);
			}
		}
		else
		{
			std::cerr << "Error in location conf" << std::endl;
			exit(1);
		}
	}
	server[ind_serv].setServLocation(tmp);
}

void ServerSocket::readConfigFile(const std::string &configFile)
{
	int index = 0;
	int ind_serv = -1;
	servers tmp;
	int inside = 0;
	int trigger;
	int bracket_counter = 0;
	std::ifstream file(configFile.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error opening configuration file" << std::endl;
		exit(1);
	}
	if (file.peek() == EOF)
	{
		std::cerr << "Error: configuration file empty" << std::endl;
		exit(1);
	}
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string key, value;
		if (line.find("server") == 0)
		{
			if (inside == 1)
			{
				std::cerr << "Error: 2 wrong config format" << std::endl;
				exit(1);
			}
			inside = 1;
			ind_serv++;
			if (line.find("{") != std::string::npos)
				bracket_counter++;
			else
			{
				std::cerr << "Error: 3 wrong config format" << std::endl;
				exit(1);
			}
			continue;
		}
		if (iss >> key)
		{
			if (key == "}")
			{
				inside = 0;
			}
			else if (key == "listen")
			{
				if (inside == 0)
				{
					std::cerr << "Error: 4 wrong config format" << std::endl;
					exit(1);
				}
				if (iss >> value)
				{
					if (checkValue(value))
					{
						double number = atof(value.c_str());
						if (number >= 1024 && number <= 65535)
							server[ind_serv].setPorts(number);
						else
						{
							std::cerr << "Error in ports config" << std::endl;
							exit(1);
						}
					}
				}
				else
				{
					std::cerr << "Error in ports config" << std::endl;
					exit(1);
				}
				if (iss >> value)
				{
					std::cerr << "Error: too many arguments in one line in conf" << std::endl;
					exit(1);
				}
			}
			else if (key == "error_page")
			{
				if (inside == 0)
				{
					std::cerr << "Error: 5 wrong config format" << std::endl;
					exit(1);
				}
				if (iss >> key)
				{
					if (iss >> value)
					{
						if (!checkValue(key))
						{
							std::cerr << "Error: 6 wrong config format" << std::endl;
							exit(1);
						}
						server[ind_serv].setServError(key, value);
					}
					else
					{
						std::cerr << "Error: 7 wrong error_page format" << std::endl;
						exit(1);
					}
				}
				else
				{
					std::cerr << "Error: 8 wrong error_page format" << std::endl;
					exit(1);
				}
				if (iss >> value)
				{
					std::cerr << "Error: too many arguments in one line in conf" << std::endl;
					exit(1);
				}
			}
			else if (key == "location")
			{
				if (inside == 0)
				{
					std::cerr << "Error: 9 wrong config format" << std::endl;
					exit(1);
				}
				trigger = 0;
				std::vector<std::string> tmpLine;
				if (line.find("{") == std::string::npos)
				{
					std::cerr << "Error: 10 wrong config format" << std::endl;
					exit(1);
				}
				tmpLine.push_back(line);
				while (std::getline(file, line))
				{
					if (line.find("{") != std::string::npos)
					{
						std::cerr << "Error: 11 wrong config format" << std::endl;
						exit(1);
					}
					if (line.find("}") != std::string::npos)
					{
						bracket_counter++;
						trigger = 1;
						break;
					}
					tmpLine.push_back(line);
				}
				if (trigger == 0)
				{
					std::cerr << "Error: 12 wrong config format" << std::endl;
					exit(1);
				}
				parseLocation(tmpLine, index, ind_serv);
				index++;
			}
			else
			{
				if (inside == 0)
				{
					std::cerr << "Error: 13 wrong config format" << std::endl;
					exit(1);
				}
				if (iss >> value)
					server[ind_serv].setServConf(key, value);
				else
				{
					std::cerr << "Error: 14 wrong config format" << std::endl;
					exit(1);
				}
				if (key == "client_max_body_size")
				{
					if (!checkValue(value))
					{
						std::cerr << "Error: 15 wrong config format" << std::endl;
						exit(1);
					}
				}
				if (iss >> value)
				{
					std::cerr << "Error: too many arguments in one line in conf" << std::endl;
					exit(1);
				}
			}
		}
		if (line.find("{") != std::string::npos || line.find("}") != std::string::npos)
			bracket_counter++;
	}
	if (bracket_counter % 2 != 0 || bracket_counter == 0)
	{
		std::cerr << "Error: 16 wrong config format" << std::endl;
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < servSize; i++)
	{
		if (server[i].getServConf("web_root") == server[i].getConfEnd())
		{
			std::cerr << "Missing web_root in Server number " << i + 1 << std::endl;
			exit(1);
		}
	}
	file.close();
}