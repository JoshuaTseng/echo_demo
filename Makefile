PROJECT_DIR=/Users/joshua/Git/echo_server

default: help

help:
	@echo "Plz check Makefile" 

run_server_job:
	./bin/echo_server 127.0.0.1 8888 &

build_server:
	gcc ./source/libs/*.c ./source/server/main.c -o ./bin/echo_server

build_client:
	gcc ./source/libs/*.c ./source/client/main.c -o ./bin/echo_client

mem_check:
	valgrind --leak-check=full --show-leak-kinds=all --verbose ./bin/echo_server 127.0.0.1 2000

env_show:
	@echo PROJECT_DIR=$(PROJECT_DIR)

docker_clean_all_container:
	docker rm $(shell docker ps -aq)

docker_run_debian_10:
	docker run -it -v $(PROJECT_DIR):/source --name debian_10 debian:10