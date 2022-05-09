# docker

> https://docker-py.readthedocs.io/en/stable/

## 安装

```bash
pip3 install docker
```

## 例子

```python
from typing import Container
import docker
import time

SCANNER_IMAGE = 'registry.sec.navinfo.com/nisec-scanner:v0.1'

API_PORT='80'

############################### Docker 磁盘映射 ################################

HOST_DIR='/opt/db'     # 宿主机目录
CONTAINER_DIR='/db'    # 容器挂载目录

########################### Docker 内存和CPU使用限制 ###########################
# https://docs.docker.com/config/containers/resource_constraints/
# docker 使用内存限制
CONTAINER_MEMORY_LIMIT='8g'

# docker 使用 CPU限制,`100000`表示1秒, `800000`表示相当于8核
CONTAINER_CPU_PERIOD=100000
CONTAINER_CPU_QUOTA=800000

###############################################################################

client = docker.from_env()

def start(port, dns=None):

    try:
        container = client.containers.run(image=SCANNER_IMAGE,        # 镜像名称
                                    detach=True,                      # 后台运行
                                    auto_remove=True,                 # docker 退出后自动删除
                                    ports={API_PORT + '/tcp': port},  # 端口映射
                                    mem_limit=CONTAINER_MEMORY_LIMIT, # 使用内存限制
                                    cpu_period=CONTAINER_CPU_PERIOD,  # 使用CPU限制
                                    cpu_quota=CONTAINER_CPU_QUOTA,    # 使用CPU限制
                                    environment={'JAVA_HOME': '/op/java'},       # 设置环境变量
                                    dns=dns,                          # 设置DNS, 类型为 list
                                    volumes={HOST_DIR : {'bind': CONTAINER_DIR, 'mode': 'rw'}}) # 磁盘映射
        return container.id
    except Exception as e:
        print(e)
        return None



def stop(id):
    try:
        container = client.containers.get(id)
        container.stop()
    except Exception as e:
        print(e)




if __name__ == '__main__':

    id = start(port=8000)
    #id = start(port=8000, dns=['114.114.114.114'])
    print(id)
    time.sleep(5)
    stop(id)
```


