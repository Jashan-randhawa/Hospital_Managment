FROM ubuntu:22.04

# Install build tools
RUN apt-get update && apt-get install -y \
    g++ \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Download httplib.h (single-header HTTP library)
RUN curl -L https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h \
    -o /usr/local/include/httplib.h

WORKDIR /app

# Copy source
COPY hospital_server.cpp .
COPY Record2.dat .

# Compile
RUN g++ -std=c++17 -O2 -o hospital_server hospital_server.cpp -lpthread -I/usr/local/include

EXPOSE 8080

CMD ["./hospital_server"]
