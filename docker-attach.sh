#!/bin/sh
# docker exec -ti $@ "/bin/bash cd /src/harness"
docker exec -w /src/harness -ti aflpp "/bin/bash"