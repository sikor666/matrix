FROM ubuntu:18.04

USER root

RUN apt-get -y update
RUN apt-get -y install cmake
RUN apt-get -y install g++
RUN apt-get -y install perl
RUN apt-get -y install python
RUN apt-get -y install git
RUN apt-get -y install libboost-all-dev
RUN apt-get -y install libssl-dev
RUN apt-get -y install libgtk2.0-dev