FROM ubuntu:20.04
RUN apt-get update
RUN apt-get install -y\
        build-essential \
        cmake \
        git \
        autoconf \
        automake \
        autotools-dev \
        curl \
        python3 \
        wget \
        vim
        
RUN git clone --recursive https://github.com/google/iree.git
