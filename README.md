# Multi-Process POSIX message queues

Creating a multi-process using a fork to establish inter-process communication through message queues. 

The new process is referred to as the client and the calling process will be referred to as the server. Furthermore, Linux supports several modes of inter-process communication through sockets, pipes, etc. that have several use cases including the server-client relationship in network calls, the parent-child relationship in the spawn of new Apache servers, etc.
