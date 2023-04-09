# About The Project
### An echo server program
- Can listen to UDP messages on specific IP and port and reply a same message back to the client.
### A client program
- Can send a UDP message to your server and display the echo message. (message, IP and port are configurable)
- Support max-retry option. \
For example, if max-retry = 10, the client will resend message at most 10 times. \
The interval between
each retry should follow the exponential backoff algorithm:
Reference: https://www.baeldung.com/resilience4j-backoff-jitter \
multiplier = 2 \
base = 500ms \
max wait_interval = 8s \
- Exit program with exit code 0 after the echo message is received
- Exit program with exit code 1 if reach max-retry

# Getting started
## Prerequisites
- gcc
- make

  Run script to install all you need.
  ```sh
  ./script/init_env.sh
  ```
## Build & Test
- Debian 10
    1. Go to project root path
    2. Open Makefile and edit below line then set your project path
        ```
        PROJECT_DIR=<set your project path here>
        ...
        ```
    3. Build server with command
        ```sh
        make build_server
        ```
    4. Build client with command
        ```sh
        make build_client
        ```
    5. Run server at local with port 8888 on background with command \
    ( If you want kill server, reference command jobs to check and kill server )
        ```sh
        make run_server_job
        ```
    6. Run client with command
        ```sh
        ./bin/echo_client 127.0.0.1 8888
        ```
- OSX
    1. Install docker \
    Please reference : \
    https://docs.docker.com/desktop/install/mac-install/
    2. Install make \
    Please reference : \
    https://formulae.brew.sh/formula/make
    3. Go to project root path
    4. Open Makefile and edit below line then set your project path
        ```
        PROJECT_DIR=<set your project path here>
        ...
        ```
    5. Run in docker
        ```
        make docker_run_debian_10
        ```
    6. Now you are in debian 10, please reference \
    Build & Test > Debian 10 steps !
# Usage
