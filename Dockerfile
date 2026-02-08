# 1. Derleme için base image
FROM ubuntu:22.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

# Derleme için gerekli kütüphaneler
RUN apt update && apt install -y \
    g++ \
    cmake \
    git \
    libboost-all-dev \
    libasio-dev \
    libopencv-dev

WORKDIR /app

# Crow kütüphanesini çek
RUN git clone https://github.com/CrowCpp/Crow.git

# Tüm projeyi içeri al (assets, templates, ads.txt dahil her şey build içine girer)
COPY . .

# Projeyi derle
RUN cmake -B build -DCROW_INCLUDE_DIR=/app/Crow/include && \
    cmake --build build --target server

# 2. Runtime stage (Çalışma ortamı)
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Çalışma zamanı için gereken minimal kütüphaneler
RUN apt update && apt install -y \
    libboost-all-dev \
    libasio-dev \
    libopencv-imgcodecs4.5d \
    libopencv-core4.5d \
    libopencv-imgproc4.5d && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Gerekli klasörleri oluştur
RUN mkdir -p /app/in_files /app/out_files

# --- DOSYALARI BUILD AŞAMASINDAN KOPYALA (Hata Veren Kısım Burasıydı) ---
# Bilgisayardan değil, build konteynırından aldığımız için "not found" hatası almazsın.
COPY --from=build /app/build/server /app/server
COPY --from=build /app/assets /app/assets
COPY --from=build /app/templates /app/templates
COPY --from=build /app/db /app/db
COPY --from=build /app/ads.txt /app/ads.txt

ENV PORT 8080
EXPOSE 8080

CMD ["./server"]
