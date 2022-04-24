# docker

> https://docker-py.readthedocs.io/en/stable/

## 安装

```bash
pip3 install docker
```

## 例子

```python
import docker
import time


class ScannerContainer(object):

    def __init__(self):
        self.__client = docker.from_env()
        self.__container = None

    def run(self, image, port):
        try:
            self.__container = self.__client.containers.run(image, detach=True, auto_remove=True, ports={'80/tcp': port})
            return self.__container.id
        except Exception as e:
            return str(e)


    def stop(self, id=None):
        if id:
            self.__container = self.__client.containers.get(id)
        try:
            self.__container.stop()
        except Exception as e:
            print(e)



if __name__ == '__main__':

    sc = ScannerContainer();
    id = sc.run('docker-name:v0.1', 8000)
    print(id)
    time.sleep(5)
    sc.stop()
```


