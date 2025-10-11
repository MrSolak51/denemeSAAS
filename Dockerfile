# 1. Derleme için uygun bir base image kullan
FROM ubuntu:22.04 AS build

RUN apt update && apt install -y g++ cmake git libboost-all-dev libasio-dev

WORKDIR /app

# Crow'u çek
RUN git clone https://github.com/CrowCpp/Crow.git
COPY . .

# Crow header'ını include path'e ekle
RUN cmake -B build -DCROW_INCLUDE_DIR=/app/Crow/include && \
# asdf
COPY . .

# 2. Projeyi derle
RUN cmake -B build && cmake --build build --target server

# 3. Runtime stage (küçük boyutlu)
FROM ubuntu:22.04

RUN apt update && apt install -y libboost-all-dev libasio-dev
WORKDIR /app

COPY --from=build /app/build/server /app/server

# Render 8080 portunu dinler
ENV PORT 8080
CMD ["./server"]
