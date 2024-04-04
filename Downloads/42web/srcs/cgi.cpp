#include "webserv.hpp"
#include <sys/types.h> // Include for pid_t
#include <sys/wait.h> // Include for waitpid

// Your existing code



/*
This function executes scripts that have a shebang. In our case, it's cgi.php but it can be .py or .pl.
*/
std::string ServerSocket::executeCGIScript(const std::string &shebang, const std::string &cgiScriptPath, const std::string &body, const std::string &filename)
{
	// Declare a string to store the response data
	std::string response_data;

	// Declare pipes for stdin and stdout
	int stdin_pipe[2];
	int stdout_pipe[2];

	// Allocate memory for the argument vector (argv)
	char **argv = (char **)malloc(sizeof(char *) * 3);
	if (argv == NULL)
	{
		// Error handling: if memory allocation fails, return an error response
		checkFdSets();
		return (callErrorFiles(500));
	}

	// Declare a pointer to hold the environment variables (envp)
	char **envp;

	// Construct the full path to the CGI script
	std::string path;
	std::map<std::string, std::string>::iterator it = currentServ.getServConf("web_root");
	if (it != currentServ.getConfEnd())
		path = it->second + cgiScriptPath;
	else
		path = cgiScriptPath;

	// Set the arguments for the CGI script execution
	argv[0] = strdup(shebang.c_str());
	argv[1] = strdup(path.c_str());
	argv[2] = 0;

	// Set the environment variables based on the request method (GET or POST)
	if (filename[0] == 0)
	{
		// GET request
		envp = (char **)malloc(sizeof(char *) * 7);
		if (envp == NULL)
		{
			// Error handling: if memory allocation fails, return an error response
			checkFdSets();
			return (callErrorFiles(500));
		}
		envp[0] = strdup("REQUEST_METHOD=GET");
		envp[6] = 0;
	}
	else
	{
		// POST request
		envp = (char **)malloc(sizeof(char *) * 8);
		if (envp == NULL)
		{
			// Error handling: if memory allocation fails, return an error response
			checkFdSets();
			return (callErrorFiles(500));
		}
		envp[0] = strdup("REQUEST_METHOD=POST");
		envp[6] = strdup((std::string("FILENAME=").append(filename)).c_str());
		envp[7] = 0;
	}

	// Set the remaining environment variables
	envp[1] = strdup((std::string("CONTENT_LENGTH=").append(bufferSize)).c_str());
	envp[2] = strdup(std::string("PATH_INFO=").append(getPathInfo(currentPath)).c_str());
	envp[3] = strdup("PATH_TRANSLATED=");
	envp[4] = strdup(std::string("QUERY_STRING=").append(getQueryString(currentPath)).c_str());
	envp[5] = strdup(std::string("SERVER_NAME=").append(serverName).c_str());

	// Create pipes for stdin and stdout
	if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1)
	{
		perror("In pipe");
		checkFdSets();
		return (callErrorFiles(500));
	}

	// Fork a new process to execute the CGI script
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("In fork");
		checkFdSets();
		return (callErrorFiles(500));
	}

	// Child process
	if (pid == 0)
	{
		// Redirect stdin and stdout to the pipes
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);
		dup2(stdin_pipe[0], STDIN_FILENO);
		dup2(stdout_pipe[1], STDOUT_FILENO);

		// Execute the CGI script
		execve(argv[0], argv, envp);
		perror("In execve");
		exit(EXIT_FAILURE);
		return "";
	}
	else
	{
		// Parent process
		close(stdin_pipe[0]);
		close(stdout_pipe[1]);

		// Write the request body to the CGI script's stdin
		write(stdin_pipe[1], body.c_str(), body.size());
		close(stdin_pipe[1]);

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		// Prepare the response header
		response_data.clear();
		response_data.append("HTTP/1.1 200 OK\r\n\r\n");

		int bytes_read;
		while ((bytes_read = read(stdout_pipe[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			// Append the CGI script's output to the response data
			response_data.append(buffer, bytes_read);
			memset(buffer, 0, bytes_read);
		}

		if (bytes_read == -1)
		{
			std::cerr << "Couldn't read any output" << std::endl;
			checkFdSets();
			return (callErrorFiles(500));
		}

		int status;
		waitpid(pid, &status, 0);

		// Free the memory allocated for argv and envp
		mfree(argv);
		mfree(envp);

		// Check the CGI script's exit status
		status = status / 256;
		if (status == 1)
			return (callErrorFiles(404));
		if (status == 255)
			return (callErrorFiles(400));
	}

	return response_data;
}