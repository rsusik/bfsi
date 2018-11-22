FROM ubuntu:17.10
ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /app

RUN apt-get update \
    && apt-get install -y --no-install-recommends apt-utils \
	&& apt-get -y install python3 \
	&& apt-get -y install make \
	&& apt-get -y install gcc \
	&& apt-get -y install g++

COPY . /app

RUN cd /app && make all

ENTRYPOINT ["python3", "test.py"]



