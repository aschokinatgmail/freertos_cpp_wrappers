FROM ubuntu:22.04

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
    clang-format \
    libgtest-dev \
    libgmock-dev \
    pkg-config \
    python3 \
    python3-pip \
    python3-markdown \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /project

# Copy dependency specs first for better layer caching
COPY CMakeLists.txt .
COPY include/ include/
COPY src/ src/
COPY tests/ tests/

RUN mkdir -p build && cd build && \
    cmake -DENABLE_COVERAGE=ON .. && \
    make -j$(nproc)

# Copy scripts after build (changes less often)
COPY scripts/ scripts/

WORKDIR /project/build

CMD ["ctest", "--verbose", "--output-on-failure"]