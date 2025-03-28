# OpenMPI ve OpenMP içeren temel bir Ubuntu imajı
FROM ubuntu:latest

# Gerekli paketleri yükleyelim
RUN apt-get update && apt-get install -y \
    openmpi-bin \
    libopenmpi-dev \
    gcc \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

COPY main.c /home/main.c

WORKDIR /home

RUN mpicc -fopenmp main.c -o main.out

CMD ["mpirun", "--allow-run-as-root", "-np", "4", "./main.out"]
