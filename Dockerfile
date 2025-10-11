# 1. Derleme için uygun bir base image kullan
FROM ubuntu:22.04 AS build

RUN apt update && apt install -y g++ cmake git libboost-all-dev libasio-dev

WORKDIR /app
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
