PROJECT_DIR=/Users/joshua/Git/echo_server

default: help

help:
	@echo "Plz check Makefile" 

env_show:
	@echo PROJECT_DIR=$(PROJECT_DIR)

docker_clean_all_container:
	docker rm $(docker ps -aq)

docker_run_debian_10:
	docker run -it -v $(PROJECT_DIR):/source --name debian_10 debian:10