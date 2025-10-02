FROM debian:bullseye

# Gerekli paketler
RUN apt-get update && apt-get install -y \
    g++ cmake git make curl && \
    rm -rf /var/lib/apt/lists/*

# Çalışma dizini
WORKDIR /app

# Crow indir
RUN curl -L https://github.com/ipkn/crow/releases/download/v0.3/crow_all.h -o crow_all.h

# Projeyi kopyala
COPY . .

# Build
RUN cmake . && make

# Render'ın default portu
ENV PORT=8080

# Çalıştır
CMD ["./hello"]
