# ridf-decoder
ROOT independent .ridf format decoder

## Recommended environment
Ubuntu 22.04 LTS

## Dependencies
Arrow, Parquet, librdkafka
```
sudo apt update
sudo apt install librdkafka-dev
sudo apt install -y -V ca-certificates lsb-release wget
wget https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
sudo apt install -y -V ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
sudo apt update
sudo apt install -y -V libarrow-dev
sudo apt install -y -V libparquet-dev
```

## Installation
- Install C++/CMake build enviroment.
- mkdir build; cd build
- cmake .. -DCMAKE_INSTALL_PREFIX=[your_installation_dir]
- make
- make install
- add installation_dir/bin to PATH

## Programs
- `ridf2parquet`
  converts ridf file to parquet file
- `ridf2stream`
  streams ridf raw byte block to Apache Kafka server
- `ridfstream_decoder`
  read ridf2stream stream and streams decoded record batches

Execute them without arguments to print usage.
