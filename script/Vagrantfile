# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|

  config.vm.define "trusty32", autostart: false do |trusty32|
    trusty32.vm.box = "ubuntu/trusty32"
    config.vm.provision "shell", path: "install_deps.sh", args: "ubuntu"
  end

  config.vm.define "freebsd", autostart: false do |freebsd|
    freebsd.vm.box_url = "http://iris.hosting.lv/freebsd-10.1-amd64.box"
    freebsd.vm.box = "freebsd-10.1"
    config.vm.provision "shell", path: "install_deps.sh", args: "freebsd"
  end

end
