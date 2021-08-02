# QR Code

二维码又称二维条码(2-dimensional bar code),，是用某种特定的几何图形按一定规律在平面（二维方向上）分布的黑白相间的图形记录数据符号信息的一种编码方式。
常见的一种二维码是 QR Code(Quick Response Code)，是一个近几年来移动设备上流行的一种编码方式，它比传统的 Bar Code 条形码能存更多的信息，也能表示更多的数据类型。

```bash
# 安装二维码解析和生成的工具，用于读取二维码并在命令行输出
sudo apt-get install qrencode zbar-tools

# 解析图片中二维码信息
zbarimg qr_code.png > qrcode.txt

# 将信息生成二维码显示在终端上
qrencode -r qrcode.txt -o - -t UTF8
```

http://zbar.sourceforge.net/