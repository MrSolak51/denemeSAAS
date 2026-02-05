# 1. Derleme için base image
FROM ubuntu:22.04 AS build

# Bu satırı ekliyoruz:
ENV DEBIAN_FRONTEND=noninteractive

# OpenCV geliştirme kütüphanelerini ekledik (libopencv-dev)
RUN apt update && apt install -y \
    g++ \
    cmake \
    git \
    libboost-all-dev \
    libasio-dev \
    libopencv-dev

WORKDIR /app

# 2. Crow'u indir
RUN git clone https://github.com/CrowCpp/Crow.git

# 3. Local projeyi kopyala
COPY . .

# 4. Projeyi derle
RUN cmake -B build -DCROW_INCLUDE_DIR=/app/Crow/include && \
    cmake --build build --target server

# 5. Runtime stage
FROM ubuntu:22.04
# Çalışma zamanında da OpenCV'nin runtime kütüphanelerine ihtiyaç var
RUN apt update && apt install -y \
    libboost-all-dev \
    libasio-dev \
    libopencv-imgcodecs4.5d \
    libopencv-core4.5d \
    libopencv-imgproc4.5d

# Runtime stage kısmına ekle
COPY assets /app/assets

RUN mkdir -p /app/in_files
RUN mkdir -p /app/out_files
RUN mkdir -p /app/db
RUN mkdir -p /app/templates

WORKDIR /app

# Derlenen binary dosyasını kopyala
COPY --from=build /app/build/server /app/server
# Mustache şablonlarını ve DB klasörünü de kopyalamayı unutma!
COPY --from=build /app/templates /app/templates
COPY --from=build /app/db /app/db

ENV PORT 8080
CMD ["./server"]