FROM lockblox/orthodox:latest
MAINTAINER Jonathan Brooker <jonathan.brooker@gmail.com>

RUN apt-get update \
 && apt-get install -y --no-install-recommends fuse libfuse-dev \
 && apt-get -y autoremove \
 && apt-get -y clean
