# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/trusty64"
  config.vm.synced_folder "./", "/vagrant"

  # enable USB in the vagrant
  config.vm.provider "virtualbox" do |vb|
    vb.customize ["modifyvm", :id, "--usb", "on"]
    vb.customize ["modifyvm", :id, "--usbehci", "on"]
  end

  # add the SEGGER J-Link device
  # add the gboot bootloader device
  config.vm.provider "virtualbox" do |vb|
    vb.customize ["usbfilter", "add", "0", "-target", :id, "-name", "jlink", "--vendorid", "1366"]
    vb.customize ["usbfilter", "add", "0", "-target", :id, "-name", "gboot", "--vendorid", "10c4"]
  end

  config.vm.provision "shell", path: "setup.sh"
end