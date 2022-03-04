qemu-img create disks/disk.img 1G
vhc --mbr disks/disk.img --directory ./root --filesystem fat32 --size 128
mv MyHDD.vmdk disks/
mv MyHDD-flat.vmdk disks/