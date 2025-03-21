# **Stage 1: Build the application**
FROM alpine:latest AS build

# Set the working directory
WORKDIR /build

# Install dependencies needed for building
RUN apk add --no-cache \
    clang \
    cmake \
    gcc \
    g++ \
    cmake \
    make \
    git \
    musl-dev \
    libc-dev \
    ncurses-dev \
    alsa-lib \
    alsa-lib-dev \
    portaudio-dev \
    portmidi-dev

# Ensure Git submodules are available inside Docker
RUN git config --global --add safe.directory '*'

# Copy source code and submodules
COPY Makefile CMakeLists.txt ./
COPY . .
RUN make clean-all && make build && make

# **Stage 2: Create the final, minimal runtime container**
FROM alpine:latest

# Set the working directory
WORKDIR /app

# Install only the runtime dependencies
RUN apk add --no-cache \
    ncurses \
    alsa-lib \
    alsa-utils \
    portaudio \
    portmidi \
    udev

# Copy only the compiled binary from the build container
COPY --from=build /build/bin/lunar-strike /app/lunar-strike

RUN chmod +x /app/lunar-strike

# Set the command to run the application
CMD ["/app/lunar-strike"]

