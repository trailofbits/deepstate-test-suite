FROM deepstate:latest

MAINTAINER Alan Cao

RUN userdd -ms /bin/bash orchestrator && echo "orchestrator:orchestrator" \
	| chpasswd && adduser orchestrator sudo

USER orchestrator
WORKDIR /home/orchestrator
COPY . /home/orchestrator/server

RUN "python3 setup.py install"
