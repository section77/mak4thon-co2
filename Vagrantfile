Vagrant.configure("2") do |config|
#  config.vm.box = "bento/ubuntu-18.04"
  config.vm.box = "bento/ubuntu-20.04"
  config.vm.provision "shell", path: "setup-ubuntu-x86.sh", env: {"IN_VAGRANT" => "true"}
end
