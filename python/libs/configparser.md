ConfigParser模块在Python3修改为configparser，这个模块定义了一个ConfigeParser类，该类的作用是让配置文件生效。配置文件的格式和window的ini文件相同，大致如下：

```ini
[section]
name1 = value1
name2:value2
```

用 = 或 ： 来赋值,section可以理解为一个模块，配置文件可以包含一个或多个 section.

该模块主要使用模块中RawConfigParser()，ConfigParser()、SafeConfigParse()这三个方法（三选一），创建一个对象使用对象的方法对配置文件进行增删改查操作.

## 写配置文件

```python
import configparser

 #类中一个方法,实例化一个对象
config = configparser.ConfigParser()
config["DEFAULT"] = {'ServerAliveInterval': '45',
                      'Compression': 'yes',
                     'CompressionLevel': '9',
                     'ForwardX11':'yes'
                     }    #类似于操作字典的形式

config['bitbucket.org'] = {'User':'Atlan'} #类似于操作字典的形式

config['topsecret.server.com'] = {'Host Port':'50022','ForwardX11':'no'}

with open('example.ini', 'w') as configfile:

   config.write(configfile)    #将对象写入文件
```

写入的example.ini如下

```ini
[DEFAULT]
serveraliveinterval = 45
compression = yes
compressionlevel = 9
forwardx11 = yes

[bitbucket.org]
user = Atlan

[topsecret.server.com]
host port = 50022
forwardx11 = no
```

## 读配置文件

注意：`configparser`有`default_section`的概念,默认为`[DEFAULT]`节,也就是之后的所有的`section`都有该默认`section`中的键值对,详情参见`configparser`源码的`__init__()`方法.

```python
import configparser

config = configparser.ConfigParser()

#---------------------------查找文件内容,基于字典的形式

config.read('example.ini')

# 打印 section 列表, 不会输出默认节 [DEFAULT]
print(config.sections())     #['bitbucket.org', 'topsecret.server.com']

print('bytebong.com' in config) # False
print('bitbucket.org' in config) # True


print(config['bitbucket.org']["user"])  # Atlan

print(config['DEFAULT']['Compression']) #yes

print(config['topsecret.server.com']['ForwardX11'])  #no


print(config['bitbucket.org'])          #<Section: bitbucket.org>

for key in config['bitbucket.org']:     # 注意,有default会默认default的键
    print(key)

print(config.options('bitbucket.org'))  # 同for循环,找到'bitbucket.org'下所有键

print(config.items('bitbucket.org'))    #找到'bitbucket.org'下所有键值对

print(config.get('bitbucket.org','compression')) # yes       get方法Section下的key对应的value
```
