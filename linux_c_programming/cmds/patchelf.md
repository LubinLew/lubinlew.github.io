# patchelf

替换二进制文件(或者so)中的依赖库

```bash
BINARY=some_binary
OLD_LIBRARY=libold.so
NEW_LIBRARY=libnew.so

if readelf -d ${BINARY} | grep 'libsome.so' &>/dev/null ; then
  patchelf --replace-needed ${OLD_LIBRARY} ${NEW_LIBRARY} ${BINARY}
fi
```