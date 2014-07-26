#! /bin/bash

echo "Sudo is required for installing dependancies and uninstalling old versions of Irrlicht"

# Dependancies
sudo apt-get install -y subversion build-essential xserver-xorg-dev x11proto-xf86vidmode-dev libxxf86vm-dev
sudo apt-get install -y mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev libxext-dev libxcursor-dev
sudo apt-get remove -y libirrlicht-dev

# Download Irrlicht
svn checkout svn://svn.code.sf.net/p/irrlicht/code/trunk irrlicht-trunk

# Build Irrlicht
cd irrlicht-trunk/source/Irrlicht
make

# Install Irrlicht
cd ../..
sudo cp -r include/* /usr/local/include/
sudo cp lib/Linux/libIrrlicht.a /usr/local/lib 
