FROM debian:bullseye

# Gerekli paketler
RUN apt-get update && apt-get install -y \
    g++ cmake git make curl && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Projeyi kopyala
COPY . .

# Build
RUN cmake . && make


# Render'ın default portu
ENV PORT=8080

# Çalıştır
CMD ["/crow/crow_examples/example"]
