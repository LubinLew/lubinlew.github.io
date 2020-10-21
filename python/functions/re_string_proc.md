# 输出正则表达式字符串

如果正则表达式中有`\r`或者 `\n`，如果想要将其写入文件或者打印时，会被当做回车换行，例如下面这样：

```python
#!/usr/bin/python3
# -*- coding: UTF-8 -*-

re_str = 'some[\r\n]text'
print(re_str)

'''输出结果错误
some[
]text
'''
```

正确方法, 是在字符串前加上 `r` 字符

```python
#!/usr/bin/python3
# -*- coding: UTF-8 -*-

re_str = r'some[\r\n]text'
print(re_str)

'''输出结果正确
some[\r\n]text
'''
```

或者下面这种方式(推荐上一种方法)

```python
#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import re

re_str = 'some[\r\n]text'
re_str = re.sub(r"\x0A", "\\\\n", re_str)
re_str = re.sub(r"\x0D", "\\\\r", re_str)

print(re_str)

'''输出结果正确
some[\r\n]text
'''
```


