#include "webserv.hpp"

/**
	- Checks deny,
	then allow conditions on matching locations
	-		Returns allow or configured deny code
	-		No match denies the request

**/

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

/**
 * Parse HTTP request -> Extract method and path.
Construct absolute path -> Append requested path to server's web root.
Check if path is a directory -> If yes, append a slash.
Start iteration over server's locations.
Check if location matches path -> If yes, check if access is denied.
If access is denied -> Return HTTP status code.
If access is not denied -> Check if HTTP method is allowed.
If method is allowed -> Return 1.
If no matching location or method not allowed -> Return 0.
*/

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
	return 0;
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
    // We're checking if the path is just a "/", which is like the front door of a website.
    if (path.length() == 1)
    {
        // Now we're going to look at all the different rooms (or 'locations') in our website.
        for (int i = 0; i < currentServ.getLocationSize(); i++)
        {
            // We're asking, "Hey, what's this room's name?"
            std::map<std::string, std::string>::iterator it = currentServ.getServLocation(i, "location");

            // If we find a room that's also called "/", which is like the main hall of our website,
            if (it != currentServ.getLocationEnd(i) && it->second == "/")
            {
                // We ask, "What's the first thing (or 'default file') people should see in this room?"
                it = currentServ.getServLocation(i, "default_file");

                // If there's something to show, we add it to our path (like guiding them to a painting in the room)
                // and raise our flag (or 'trigger') to say we've found something.
                if (it != currentServ.getLocationEnd(i))
                {
                    path = path + it->second;
                    trigger = 1;
                }
            }
        }
    }
}

void ServerSocket::handleDirectoryOrFile(std::string &path, int &trigger)
{
    struct stat s;

    // Check if the path exists
    if (stat(path.c_str(), &s) == 0)
    {
        // If the path is a directory
        if (s.st_mode & S_IFDIR)
        {
            // Iterate over all the server's locations
            for (int i = 0; i < currentServ.getLocationSize(); i++)
            {
                // Get the location configuration
                std::map<std::string, std::string>::iterator it = currentServ.getServLocation(i, "location");

                // If the location configuration exists and matches the path
                if (it != currentServ.getLocationEnd(i) && it->second == path)
                {
                    // Check if autoindex is on for this location
                    it = currentServ.getServLocation(i, "autoindex");

                    // If autoindex is on, append "index.html" to the path and set trigger to 1
                    if (it != currentServ.getLocationEnd(i) && it->second == "on")
                    {
                        path = path + "index.html";
                        trigger = 1;
                    }
                }
            }
        }
        // If the path is a regular file, set trigger to 1
        else if (s.st_mode & S_IFREG)
        {
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

/*
The function is like a gatekeeper. 
It reads a list of instructions (the configuration file) for setting up servers. It checks each instruction carefully to make sure it's valid and makes sense.
If it finds an instruction that's wrong or doesn't make sense, it stops and reports an error. 
If all the instructions are good, it sets up the servers according to the instructions.
*/

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