FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    g++ \
    clang-tidy \
    cppcheck \
    doxygen \
    graphviz \
    lcov \
    gcovr \
    clang-format \
    libgtest-dev \
    libgmock-dev \
    pkg-config \
    python3 \
    python3-pip \
    python3-markdown \
    python3-kconfiglib \
    git \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    qemu-system-arm \
    && rm -rf /var/lib/apt/lists/*

RUN cd /usr/src/gtest && cmake . && make -j$(nproc) && cp lib/*.a /usr/lib/ || true
RUN cd /usr/src/gmock && cmake . && make -j$(nproc) && cp lib/*.a /usr/lib/ || true

WORKDIR /project

CMD ["bash"]