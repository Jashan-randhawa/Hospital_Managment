FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install g++ and httplib directly from apt (no download needed)
RUN apt-get update && apt-get install -y \
    g++ \
    libcpp-httplib-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source files
COPY hospital_server.cpp .
COPY Record2.dat .

# Compile
RUN g++ -std=c++17 -O2 -o hospital_server hospital_server.cpp -lpthread -lcpp-httplib

EXPOSE 8080

CMD ["./hospital_server"]
