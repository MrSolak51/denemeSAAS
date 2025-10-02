FROM debian:bullseye

# Gerekli paketler
RUN apt-get update && apt-get install -y \
    g++ cmake git make curl && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Crow indir
RUN git clone https://github.com/CrowCpp/Crow.git /crow

# Projeyi kopyala
COPY . .

# Build
RUN cmake -DCROW_MAIN=ON -DCMAKE_BUILD_TYPE=Release /crow && cmake --build /crow

# Render'ın default portu
ENV PORT=8080

# Çalıştır
CMD ["/crow/crow_examples/example"]
