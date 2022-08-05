FROM ubuntu:18.04

# Default git repository
ENV GIT_REPOSITORY https://github.com/scala-network/XLArig.git
ENV XMRSTAK_CMAKE_FLAGS -DXLArig_COMPILE=generic -DCUDA_ENABLE=ON -DOpenCL_ENABLE=OFF

# Install packages
RUN apt-get update \
    && set -x \
    && apt-get install -qq --no-install-recommends -y build-essential ca-certificates cmake cuda-core-9-0 git cuda-cudart-dev-9-0 libhwloc-dev libmicrohttpd-dev libssl-dev \
    && git clone $GIT_REPOSITORY \
    && cd /XLArig \
    && cmake ${XMRSTAK_CMAKE_FLAGS} . \
    && make \
    && cd - \
    && mv /XLArig/bin/* /usr/local/bin/ \
    && rm -rf /XLArig \
    && apt-get purge -y -qq build-essential cmake cuda-core-9-0 git cuda-cudart-dev-9-0 libhwloc-dev libmicrohttpd-dev libssl-dev \
    && apt-get clean -qq

VOLUME /mnt

WORKDIR /mnt

ENTRYPOINT ["/usr/local/bin/XLArig"]
