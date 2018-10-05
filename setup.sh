#!/bin/bash

sudo apt-get update > /dev/null 2>&1
sudo apt-get install -y build-essential > /dev/null 2>&1
sudo apt-get install -y autoconf > /dev/null 2>&1
sudo apt-get install -y automake > /dev/null 2>&1

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test > /dev/null 2>&1
sudo apt-get update > /dev/null 2>&1
sudo apt-get upgrade > /dev/null 2>&1
sudo apt-get dist-upgrade > /dev/null 2>&1

sudo dpkg --add-architecture i386 > /dev/null 2>&1
sudo apt-get update > /dev/null 2>&1
sudo apt-get install -y libc6:i386 libncurses5:i386 libstdc++6:i386 > /dev/null 2>&1

sudo apt-get install -y python-pip > /dev/null 2>&1
sudo pip install intelhex > /dev/null 2>&1
sudo apt-get install -y srecord > /dev/null 2>&1
sudo apt-get install -y cmake > /dev/null 2>&1
sudo apt-get install -y libusb-1.0-0-dev > /dev/null 2>&1

wget http://download2.nust.na/pub4/sourceforge/s/project/sd/sdcc/sdcc-linux-x86/3.6.0/sdcc-3.6.0-i386-unknown-linux2.5.tar.bz2 > /dev/null 2>&1
tar xjf sdcc-3.6.0-i386-unknown-linux2.5.tar.bz2 > /dev/null 2>&1
sudo cp -r sdcc-3.6.0/* /usr/local
rm -rf sdcc-3.6.0
rm -rf sdcc-3.6.0-i386-unknown-linux2.5.tar.bz2

if [ -f /vagrant/JLink_Linux_*.deb ]; then
    sudo dpkg -i /vagrant/JLink_Linux_*.deb > /dev/null 2>&1
fi

echo "All Finished!"