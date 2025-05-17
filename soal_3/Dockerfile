FROM ubuntu:20.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    fuse libfuse2 libfuse-dev gcc pkg-config

COPY antink.c /root/antink.c

RUN gcc -Wall -D_FILE_OFFSET_BITS=64 `pkg-config fuse --cflags` /root/antink.c -o /root/antink `pkg-config fuse --libs`

ENTRYPOINT ["/root/antink"]