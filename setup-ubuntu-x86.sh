#!/bin/sh
echo "Starting unattended installation"
export DEBIAN_FRONTEND=noninteractive
export APT_KEY_DONT_WARN_ON_DANGEROUS_USAGE=1
sudo apt-get update
sudo apt-get -y install \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg-agent \
    software-properties-common

echo "downloading gpg key from docker"
curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o docker-gpg-key

echo "adding gpg key"
sudo apt-key add docker-gpg-key

echo "adding repository"
sudo add-apt-repository \
    "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
    $(lsb_release -cs) \
    stable"

echo "downloading necessary packages for docker"
sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io

echo "installing docker compose"
sudo curl -L "https://github.com/docker/compose/releases/download/1.27.4/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose

if [ -n "$IN_VAGRANT" ]; then
    echo "We are in vagrant. Execute docker-compose"
    cd /vagrant/
    sudo docker-compose up -d
else
    echo "To start containers execute docker-compose up within project root."
fi  