# 1. Derleme için base image
FROM ubuntu:22.04 AS build

RUN apt update && apt install -y g++ cmake git libboost-all-dev libasio-dev

WORKDIR /app

# 2. Crow'u indir
RUN git clone https://github.com/CrowCpp/Crow.git

# 3. Local projeyi kopyala
COPY . .

# 4. Projeyi derle
RUN cmake -B build -DCROW_INCLUDE_DIR=/app/Crow/include && cmake --build build --target server

# 5. Runtime stage
FROM ubuntu:22.04
RUN apt update && apt install -y libboost-all-dev libasio-dev
WORKDIR /app

COPY --from=build /app/build/server /app/server

ENV PORT 8080
CMD ["./server"]
