# 42_Webserv

<div style="display: flex; justify-content: center; align-items: center; height: 100vh;">
    <img src="https://github.com/mdabir1203/WebserV/assets/130859506/7e2da4ce-256e-430f-ba5d-79bb1f47586e" width="500" style="margin: auto;">
</div>



## Conceptual Understanding (with State Diagrams)
### Leveraged LLM + Javascript with p5.js library to build the below

### Initialization
![image](https://github.com/mdabir1203/WebserV/assets/66947064/6e0690ab-0636-4849-9c3a-b81226d6c30b)

### Listening Process 

![image](https://github.com/mdabir1203/WebserV/assets/66947064/52c5907c-d41f-4525-9e59-4fca8b55fc00)

### Parsing 

![image](https://github.com/mdabir1203/WebserV/assets/66947064/6c0b7b3e-d36f-416a-9121-2fd3477829e0)

### FileServing

![image](https://github.com/mdabir1203/WebserV/assets/66947064/78ca9862-4813-4856-af46-971c174965ac)


### CGIHandling

![image](https://github.com/mdabir1203/WebserV/assets/66947064/5af898b0-3a40-4c8b-add7-47c6de0bbf89)



## Architectural Decision : 

## Decision 1: Using the High definition State Machine Parsing 
### Context

Parsing HTTP requests is a fundamental aspect of any web server. The challenge lies in efficiently handling diverse and potentially complex request formats while ensuring robustness and speed.
Option Considered

    Regex-Based Parsing: Using regular expressions to parse HTTP requests.
    State Machine Parsing: Implementing a finite state machine to sequentially process request characters and transition through states based on the input.

### Decision

Performance: State machines are generally faster and less memory-intensive than regex parsing, especially for complex patterns. This efficiency is critical for         high-throughput servers.
Maintainability: While regex can get complicated and hard to debug with complex patterns, state machines offer clearer logic and easier maintenance.
Error Handling: State machines allow for more graceful error handling and recovery, providing clear paths for dealing with malformed requests.

### Impact

    Improved server performance and responsiveness.
    Enhanced ability to handle a wide range of HTTP request formats.
    Simplified debugging and maintenance of the parsing logic.
### Reference : 
- https://hackernoon.com/state-machines-can-help-you-solve-complex-programming-problems

## Decision 2: Singleton Design Pattern
Context

Managing configuration and shared resources in a webserver is crucial for consistent behavior and performance. Ensuring that there's a single instance of the server configuration and state management component is vital.
Options Considered

    Global Variables: Using global variables to store configuration and state.
    Singleton Design Pattern: Ensuring a class has only one instance and providing a global point of access to it.

Decision

We opted for the Singleton Design Pattern due to the following:

    Controlled Access: Singleton provides a controlled mechanism to access and modify server configuration, preventing accidental overwrites or conflicts.
    Lazy Initialization: It allows for the lazy initialization of the server's configuration, loading resources only when they are needed and not before.
    Encapsulation: Encapsulates the server's state and configuration within a single, well-defined class, improving code organization and readability.

Impact

    Ensured consistent access and modification of server configuration.
    Reduced memory usage through lazy initialization.
    Improved code organization and maintainability by encapsulating server state management.

### Reference: 
-  https://refactoring.guru/design-patterns/singleton

Description

This is a simple HTTP web server implemented in C++98 as part of the 42 curriculum. The web server is capable of handling basic HTTP requests and serving static files.
Features

    Handles GET requests for static files
    Supports basic authentication
    Logs requests to standard error
    Uses epoll for efficient I/O multiplexing
    Customizable root directory

Building and Running

To build and run the web server, follow these steps:

    Clone the repository: git clone [repository URL]
    Navigate to the project directory: cd webserver
    Build the project: make
    Run the web server with the root directory set to relative path of /www: and run ./webserver 
****


## Tree Structure 

### Visualization of IPC and Sockets

    ├─ Representation of Kernel
    ├─ The kernel is represented as a red circle at the center of the canvas.
    └─ This is a reasonable representation as the kernel is the core component that manages system resources, including sockets and network connections.

### Representation of Processes  

    ├─ Processes are represented as green circles scattered around the canvas.
    └─ This is a reasonable representation as processes are separate entities running on the system.

### Representation of Sockets

    ├─ Sockets are represented as smaller blue circles inside the process circles.
    ├─ This representation suggests that sockets are created and owned by processes, which is accurate.
    └─ However, it might be more intuitive to represent sockets as separate entities from processes, as sockets are managed by the kernel and can be shared across processes.
### Representation of Connections
    ├─ Connections between sockets are represented as white lines connecting the sockets.
    └─ This is a reasonable representation as connections are established between sockets to enable communication.  
### Interaction
    ├─ The user can create sockets by clicking on processes. 
    ├─ Sockets automatically connect when they are close enough to each other.
    ├─ This interaction allows for the dynamic creation and connection of sockets, which is a good representation of how sockets are created and used in real-world scenarios.  
    └─ However, it might be more realistic to have a separate step for explicitly connecting sockets, as connections are not always automatically established based on proximity.

### Overall Representation
     
     ├─ The visualization provides a high-level representation of the key components involved in IPC and sockets: the kernel, processes, sockets, and connections.
     ├─ The visual representations and interactions are reasonably intuitive and capture the essential concepts.
     └─ However, some aspects could be improved to better align with the actual implementation details and behavior of sockets and network communication.

## Links : 

   - ![image](https://github.com/mdabir1203/WebserV/assets/66947064/87590939-ef87-41a7-b9fb-7c5cebe19987)
