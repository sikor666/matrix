# Docker
docker build --rm -t matrix .  
docker run --rm -ti -v 'code path':/workspace matrix:latest /bin/bash  

# GIT
git submodule update --init

# All
mkdir build  
cd build  

# Linux
cmake ..  
  
# Windows
cmake -G "Visual Studio 15" ..  
  