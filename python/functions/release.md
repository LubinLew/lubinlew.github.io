# python 项目打包发布

> https://zhuanlan.zhihu.com/p/114338298


1. 编写 setup.py 文件声明发布包的meta 信息，比如包的名字，模块名等

2. 打包, 执行 `python setup.py bdist_wheel` 命令, whl文件在 dist 目录下

3. 将whl文件上传到 pypi 官网发布

4. 通过 `pip install 模块名` 安装测试

