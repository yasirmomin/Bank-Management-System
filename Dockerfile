FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    pkg-config \
    libmysqlclient-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

# We will mount the project into /workspace at runtime, so don't COPY source here.
CMD ["tail", "-f", "/dev/null"]
