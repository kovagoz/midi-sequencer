# -*- mode: ruby -*-
# vi: set ft=ruby :
Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/jammy64"
  config.vm.box_check_update = false

  # config.vm.provider "virtualbox" do |vb|
  #   vb.customize ["modifyvm", :id, "--usb", "on"]
  #   vb.customize ["modifyvm", :id, "--usbxhci", "on"]
  #   vb.customize ["usbfilter", "add", "0",
  #       "--target", :id,
  #       "--name", "ESP32",
  #       "--manufacturer", "Silicon Labs",
  #       "--product", "CP2102 USB to UART Bridge Controller"]
  
  #   # Customize the amount of memory on the VM:
  #   # vb.memory = "1024"
  # end

  # config.vm.provision "shell", inline: <<-SHELL
  #   apt-get update
  #   apt-get install -y apache2
  # SHELL
end
