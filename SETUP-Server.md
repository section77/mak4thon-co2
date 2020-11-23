# Setup Ubuntu Server

Folge am besten der aktuellen Originalanleitung unter https://docs.docker.com/engine/install/ubuntu/

Zum Zeitpunkt des Schreibens (und so in Verwendung in [setup-ubuntu-x86.sh](setup-ubuntu-x86.sh)):

## OS requirements

Um Docker Engine zu installieren, benötigst du eine dieser 64-bit Versionen von Ubuntu:

Ubuntu Focal 20.04 (LTS)
Ubuntu Bionic 18.04 (LTS)
Ubuntu Xenial 16.04 (LTS)

# Alte Version(en) deinstallieren

für den Fall dass bereits irgendetwas über das standard apt repository installiert wurde:

    $ sudo apt-get remove docker docker-engine docker.io containerd runc

# Installation via docker repository

1. ) Update the apt package index and install packages to allow apt to use a repository over HTTPS:

```
    $ sudo apt-get update
    $ sudo apt-get install \
        apt-transport-https \
        ca-certificates \
        curl \
        gnupg-agent \
        software-properties-common
```

2. Add Docker’s official GPG key:

```
    $ curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
```

Verify that you now have the key with the fingerprint 9DC8 5822 9FC7 DD38 854A  E2D8 8D81 803C 0EBF CD88, by searching for the last 8 characters of the fingerprint.

```

$ sudo apt-key fingerprint 0EBFCD88

pub   rsa4096 2017-02-22 [SCEA]
      9DC8 5822 9FC7 DD38 854A  E2D8 8D81 803C 0EBF CD88
uid           [ unknown] Docker Release (CE deb) <docker@docker.com>
sub   rsa4096 2017-02-22 [S]
```

3. Use the following command to set up the stable repository. To add the nightly or test repository, add the word nightly or test (or both) after the word stable in the commands below. Learn about nightly and test channels.


x86/ amd64 (normale PCs):

```
    $ sudo add-apt-repository \
    "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
    $(lsb_release -cs) \
    stable"
```

Raspberry: https://wiki.debian.org/RaspberryPi4

armhf (raspberryPI 3/4 mit Raspbian):

```
    $ sudo add-apt-repository \
    "deb [arch=armhf] https://download.docker.com/linux/ubuntu \
    $(lsb_release -cs) \
    stable"
```

arm64 (Raspberry PI mit Ubuntu und arm64 image)

```
    $ sudo add-apt-repository \
    "deb [arch=arm64] https://download.docker.com/linux/ubuntu \
    $(lsb_release -cs) \
    stable"
```

## Docker Pakete installieren

sudo apt-get update
sudo apt-get install docker-ce docker-ce-cli containerd.io

## install docker compose

Anleitung hier: https://docs.docker.com/compose/install/

```
sudo curl -L "https://github.com/docker/compose/releases/download/1.27.4/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
```