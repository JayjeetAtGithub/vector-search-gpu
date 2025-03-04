#!/bin/bash
set -e

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

run_flat() {
    ./run_faiss_gpu \
        --index-type flat \
        --dataset-dir /workspace/dataset/deep1b \
        --learn-limit ${1} \
        --search-limit ${2} \
        --top-k 10 \
        --metric ip \
        --index-file gpu_flat_${1}l_${2}q.faiss

    ./run_faiss_gpu \
        --index-type flat \
        --dataset-dir /workspace/dataset/deep1b \
        --learn-limit ${1} \
        --search-limit ${2} \
        --top-k 10 \
        --metric ip \
        --skip-build 1 \
        --index-file gpu_flat_${1}l_${2}q.faiss \
        --calc-recall true
}

run_ivf() {
    ./run_faiss_gpu \
        --index-type ivf \
        --dataset-dir /workspace/dataset/deep1b \
        --learn-limit ${1} \
        --search-limit ${2} \
        --top-k 10 \
        --n-probe ${3} \
        --metric ip \
        --index-file gpu_ivf_${1}l_${2}q.faiss

    ./run_faiss_gpu \
        --index-type ivf \
        --dataset-dir /workspace/dataset/deep1b \
        --learn-limit ${1} \
        --search-limit ${2} \
        --top-k 10 \
        --n-probe ${3} \
        --metric ip \
        --skip-build 1 \
        --index-file gpu_ivf_${1}l_${2}q.faiss \
        --calc-recall true
}

run_flat 1000000 10000
run_ivf 1000000 10000 256
