FROM ubuntu:20.04
RUN apt-get update
RUN apt-get install -y\
        build-essential \
        cmake \
        git \
        curl \
        wget 
        
RUN git clone --recursive https://github.com/google/iree.git
