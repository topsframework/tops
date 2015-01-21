# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|

  config.vm.define "trusty32", autostart: false do |trusty32|
    trusty32.vm.box = "ubuntu/trusty32"
    config.vm.provision "shell", path: "install_deps.sh", args: "ubuntu"
  end

end
