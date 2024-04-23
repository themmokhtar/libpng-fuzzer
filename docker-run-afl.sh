#!/bin/sh
docker container prune
docker run --name aflpp -w /src/harness -ti -v .:/src aflplusplus/aflplusplus $@