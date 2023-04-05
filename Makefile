PROJECT_DIR=/Users/joshua/Git/echo_server

default: help

test:
	gcc ./source/server/main.c -o ./bin/echo_server
	./bin/echo_server 127.0.0.1 2000

build:
	gcc ./source/server/main.c -o ./bin/echo_server

help:
	@echo "Plz check Makefile" 

env_show:
	@echo PROJECT_DIR=$(PROJECT_DIR)

docker_clean_all_container:
	docker rm $(docker ps -aq)

docker_run_debian_10:
	docker run -it -v $(PROJECT_DIR):/source --name debian_10 debian:10