FROM alpine:3.14

# Update and install necessary packages
RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
    clang \
    clang-dev \
    alpine-sdk \
    dpkg \
    cmake \
    ccache \
    python3

# Set up symbolic links for clang
RUN ln -sf /usr/bin/clang /usr/bin/cc \
  && ln -sf /usr/bin/clang++ /usr/bin/c++ \
  && update-alternatives --install /usr/bin/cc cc /usr/bin/clang 10 \
  && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 10 \
  && update-alternatives --auto cc \
  && update-alternatives --auto c++ \
  && update-alternatives --display cc \
  && update-alternatives --display c++ \
  && ls -l /usr/bin/cc /usr/bin/c++ \
  && cc --version \
  && c++ --version

# Set the working directory
WORKDIR /app

# Copy necessary files
COPY ./include ./include
COPY ./lib ./lib
COPY ./src ./src
COPY ./CMakeLists.txt ./CMakeLists.txt

# Create build directory and build the project
WORKDIR /app/build
RUN cmake ..
RUN cmake --build .

# Command to run the application
CMD ["./backend"]