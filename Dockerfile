FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
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
        libmpc-dev \
        libmpfr-dev \
        libgmp-dev \
        gawk \
        bison \
        flex \
        texinfo \
        gperf \
        libtool \
        patchutils \
        bc \
        zlib1g-dev \
        libexpat-dev \
        vim \
        wget 
        
RUN git clone --recursive https://github.com/google/iree.git
