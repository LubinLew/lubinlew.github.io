# deploy-openvas

deploy on Ubuntu 18.04 LTS

## create user

```bash
useradd -r -d /opt/gvm -c "GVM (OpenVAS) User" -s /bin/bash gvm
echo 'gvm ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/gvm
chmod 440 /etc/sudoers.d/gvm
mkdir /opt/gvm
chown gvm:gvm /opt/gvm
```

## gvm-libs

```bash
# install deps
apt install -y \
gcc cmake pkg-config \
libglib2.0-dev \
libgpgme-dev \
libgnutls28-dev \
uuid-dev \
libssh-gcrypt-dev \
libhiredis-dev \
libxml2-dev \
libpcap-dev \
libradcli-dev \
libldap2-dev

# build and install gvm-libs
git clone -b gvm-libs-20.08 https://github.com/greenbone/gvm-libs.git
cd gvm-libs
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/gvm
make
make install

# add lib link path
echo "/opt/gvm/lib" > /etc/ld.so.conf.d/gvm.conf
export PKG_CONFIG_PATH=/opt/gvm/lib/pkgconfig:$PKG_CONFIG_PATH
ldconfig
```

## openvas-scanner

```bash
# install deps
apt install -y \
gcc cmake pkg-config \
libssh-gcrypt-dev \
libgnutls28-dev \
libglib2.0-dev \
libpcap-dev \
libgpgme-dev \
libksba-dev \
libsnmp-dev \
libgcrypt20-dev \
clang-format \
nmap \
bison \
redis-server

# build and install openvas-scanner
git clone -b openvas-20.08 https://github.com/greenbone/openvas.git
cd openvas
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/gvm
make
make install


# add exec path
sed -i 's/"$/:\/opt\/gvm\/bin:/opt\/gvm\/sbin"/g' /etc/environment
source /etc/environment

# set up redis
cp ../config/redis-openvas.conf  /etc/redis/
chown redis:redis /etc/redis/redis-openvas.conf
echo "db_address = /run/redis-openvas/redis.sock" > /opt/gvm/etc/openvas/openvas.conf
usermod -aG redis gvm
#### fix redis warning
echo "net.core.somaxconn = 1024" > /etc/sysctl.d/redis.conf
echo 'vm.overcommit_memory = 1' >> /etc/sysctl.d/redis.conf
sysctl -p /etc/sysctl.d/redis.conf
#### disable Transparent Huge Pages (THP) for 
apt install -y hugepages
hugeadm --thp-never
#### make a service
cat > /etc/systemd/system/thp_never.service << EOF
[Unit]
Description=Disable Kernel Support for Transparent Huge Pages (THP)

[Service]
Type=simple
ExecStart=/usr/bin/hugeadm --thp-never

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable --now thp_never

#### start 
systemctl start redis-server@openvas
systemctl enable redis-server@openvas
```

## ospd-openvas

```bash
apt install -y python3-pip
python3 -m pip install --upgrade pip
pip3 install ospd-openvas==20.8.1
```

## gvmd

```bash
# install deps
apt install -y \
gcc cmake pkg-config \
libglib2.0-dev \
libgnutls28-dev \
libpq-dev \
libical-dev \
postgresql-server-dev-all \
xsltproc \
xml-twig-tools \
gnutls-bin \
postgresql postgresql-contrib


## setup postgres
systemctl restart postgresql
sudo -Hiu postgres createuser gvm
sudo -Hiu postgres createdb -O gvm gvmd
sudo -Hiu postgres psql -c 'create role dba with superuser noinherit;' gvmd
sudo -Hiu postgres psql -c 'grant dba to gvm;' gvmd
sudo -Hiu postgres psql -c 'create extension "uuid-ossp";' gvmd
sudo -Hiu postgres psql -c 'create extension "pgcrypto";' gvmd
systemctl restart postgresql
systemctl enable postgresql


# create admin account
cat > /tmp/set_gvmd.sh << EOF
#!/bin/bash
set -x

/opt/gvm/bin/gvm-manage-certs -a

/opt/gvm/sbin/gvmd --create-user admin --password=admin
UUID=\$(/opt/gvm/sbin/gvmd  --get-users --verbose | grep admin | awk '{print \$2}')
/opt/gvm/sbin/gvmd --modify-setting 78eceaec-3385-11ea-b237-28d24461215b --value \$UUID

SCANNER=\$(/opt/gvm/sbin/gvmd --get-scanners | grep 'Default')
SCANNER_ID=\$(echo \$SCANNER | awk '{print \$1}')
SCANNER_HOST=\$(echo \$SCANNER | awk '{print \$3}')
/opt/gvm/sbin/gvmd --modify-scanner=\$SCANNER_ID --scanner-host=\$SCANNER_HOST
EOF

chown gvm:gvm /tmp/set_gvmd.sh
sudo -Hiu gvm bash /tmp/set_gvmd.sh
```

## GSA

```bash
# install deps
apt install -y \
libmicrohttpd-dev \
libxml2-dev \
libmicrohttpd-dev \
xmltoman

# install yarn
curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | apt-key add -
echo "deb https://dl.yarnpkg.com/debian/ stable main" | tee /etc/apt/sources.list.d/yarn.list
apt update
apt install -y yarn
yarn install
yarn upgrade

# install nodejs
curl -sL https://deb.nodesource.com/setup_12.x | sudo bash -
apt install nodejs

# buid and install gsa
git clone -b gsa-20.08 https://github.com/greenbone/gsa.git
cd gsa
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/gvm
make
make install
```

## tail

```bash
chown gvm:gvm -R /opt/gvm/
```

## Reference

https://github.com/yu210148/gvm_install/blob/master/install_gvm.sh

https://kifarunix.com/install-and-setup-gvm-11-on-ubuntu-20-04/

https://sadsloth.net/post/install-gvm11-src-on-debian/

