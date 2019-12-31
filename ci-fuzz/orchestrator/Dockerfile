FROM python:3.7.2-slim

MAINTAINER Alan Cao

# install necessary services
RUN apt-get update && \
	apt-get -y install netcat && \
	apt-get clean


# initialize priviledged user and home dir
RUN userdd -ms /bin/bash orchestrator && echo "orchestrator:orchestrator" | chpasswd && adduser orchestrator sudo
USER orchestrator
WORKDIR /home/orchestrator

# copy over server app and run setup
COPY . /home/orchestrator/server
RUN "python3 server/setup.py install"

# start the server
# TODO: user input config.yml
CMD ["fuzzbed-orchestrator"]
