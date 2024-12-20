# Vector Search on GPU

**NOTE**: In HNSW and IVF implementations, the default construction parameters are used. Only the search parameters can be changed. The default construction parameters are as follows:

```bash
# HNSW
M = 32
efConstruction = 40

# IVF-Flat
n_list = int64_t(4 * std::sqrt(n_learn))
```

## System Requirements

* Ubuntu 22.04+
* cmake >= 3.29
* gcc/g++ >= 13.1.0
* CUDA >= 12.4
* NVIDIA driver >= 550

## Building from Source

```bash
git clone --recursive https://github.com/JayjeetAtGithub/vector-search-gpu
cd vector-search-gpu/
./install_faiss.sh
mkdir build/
cd build/
cmake ..
make
```

## Machine Setup

**NOTE**: Update the `gcc`/`g++` first and then install the NVIDIA drivers and CUDA toolkit.

### Updating `gcc` and `g++`

```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-13 g++-13
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100
sudo update-alternatives --config gcc
sudo update-alternatives --config g++
gcc --version
g++ --version
```

### Updating `cmake`

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.30.5/cmake-3.30.5-linux-x86_64.sh
chmod +x cmake-3.30.5-linux-x86_64.sh
./cmake-3.30.5-linux-x86_64.sh
cd cmake-3.30.5-linux-x86_64
sudo cp -r bin/* /usr/local/bin/
sudo cp -r doc/* /usr/local/doc/
sudo cp -r man/* /usr/local/man/
sudo cp -r share/* /usr/local/share/
cmake --version
```

### NVIDIA Drivers and CUDA Toolkit

```bash
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-ubuntu2204.pin
sudo mv cuda-ubuntu2204.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/12.6.2/local_installers/cuda-repo-ubuntu2204-12-6-local_12.6.2-560.35.03-1_amd64.deb
sudo dpkg -i cuda-repo-ubuntu2204-12-6-local_12.6.2-560.35.03-1_amd64.deb
sudo cp /var/cuda-repo-ubuntu2204-12-6-local/cuda-*-keyring.gpg /usr/share/keyrings/
sudo apt-get update
sudo apt-get -y install cuda-toolkit-12-6
```
