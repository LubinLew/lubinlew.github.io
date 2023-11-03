# QCOW2镜像内容修改

有两种修改方式: 

- 1.修改文件 
- 2.执行命令


## 修改文件

> https://gist.github.com/shamil/62935d9b456a6f9877b5?permalink_comment_id=3608240

通过挂载 qcow2 磁盘到本地

```bash
modprobe nbd max_part=8
qemu-nbd --connect=/dev/nbd0 /var/lib/vz/images/100/vm-100-disk-1.qcow2
fdisk /dev/nbd0 -l
mount /dev/nbd0p1 /mnt/somepoint/

## cleanup
umount /mnt/somepoint/
qemu-nbd --disconnect /dev/nbd0
rmmod nbd
```

## 执行命令

```bash
## 执行命令
virt-customize -a some.qcow2 --run-command 'ssh-keygen -A'
virt-customize -a some.qcow2 --run-command 'systemctl enable ssh'

## 设置root密码
virt-customize -a some.qcow2 --root-password password:mypass

## 设置主机名
virt-customize -a some.qcow2 --hostname myhost
```