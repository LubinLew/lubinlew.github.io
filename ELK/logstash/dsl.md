# 语法

> [Structure of a Config File | Logstash Reference | Elastic](https://www.elastic.co/guide/en/logstash/current/configuration-file-structure.html)

logstash 设计了自己的DSL, logstash 自身使用 ruby 和 java 编写，很多语法与 ruby 相似。



## 区段(section)

logstash 使用 `{}` 来定义区域。区域内可以包括插件区域定义，你可以在一个区域内定义多个插件。插件区域内则可以定义键值对设置。

```logstash
input {
    stdin {}
    syslog {}
}
```

## 注释(comments)

使用 `#` 开头的行为注释, 不支持多行注释的方式

```logstash
# this is a comment

input { # comments can appear at the end of a line, too
  # ...
}
```



---

## 数据类型

logstash 支持少量的数据类型:

### 布尔(Boolean)

取值范围为 `true` 和 `false`

```logstash
debug => true
```

### 字符串

字符串使用双引号声明

```logstash
host => "hostname"
```

### 数字(Number)

数字必须是有效的数值（浮点数或整数）

```logstash
port => 514
```

### 数组(Array)

现在，不建议使用此类型，而建议使用标准类型（如string）的插件，该插件定义：list => true属性，以进行更好的类型检查。 仍然需要处理不需要类型检查的哈希表或混合类型列表。

数组使用 `[]` 声明, 多个元素之间使用 `,` 逗号分隔

```logstash
users => [ {id => 1, name => bob}, {id => 2, name => jane} ]
```

### 列表(Lists)

它本身不是类型，但属性类型可以具有。 这样就可以键入检查多个值。 声明参数时，插件作者可以通过指定：list => true来启用列表检查。

```logstash
  path => [ "/var/log/messages", "/var/log/*.log" ]
  uris => [ "http://elastic.co", "http://example.net" ]
```



### 哈希(Hash)

哈希是以“ field1” =>“ value1”格式指定的键值对的集合。 请注意，多个键值条目由空格而不是逗号分隔。

```logstash
match => {
  "field1" => "value1"
  "field2" => "value2"
  ...
}
# or as a single line. No commas between entries:
match => { "field1" => "value1" "field2" => "value2" }
```

---

## 字段引用

字段是 Logstash::Event 对象的属性。我们之前提过时间就像一个哈希一样，所以你可以想象字段就像一个键值对。

如果你想在 logstash 配置中使用字段的值，只需要把字段的名字写在中扩号`[]`里就行了,这就叫 "字段引用"。

对于“嵌套字段“(也就是多维哈希表)每层的字段都写到`[]` 中即可,例如下面从 geoip中获取数据的例子:

```logstash
[geoip][location][0]
```

> logstash 的数组也只下标的倒序, 例如 -1 表示数组的最后一个元素,如 [geoip][location][-1]

logstash 还支持变量内插, 在字符串里使用字段引用的方法如下:

```logstash
"the longitude is %{[geoip][location][0]}"
```

---

## 条件判断

logstash 从 1.3.0 版本开始支持条件判断和表达式。

表达式支持下列操作符:

| 类型    | 操作符                         |     |
| ----- | --------------------------- | --- |
| 值比较   | `==`、`!=`、`<`、`>`、`<=`、`>=` |     |
| 正则    | `=~`、`!~`                   |     |
| 包含判读  | `in`、`not in`               |     |
| 逻辑操作符 | `and`、`or`、`nand`、`xor`     |     |
| 一元操作符 | `!()`                       |     |

条件判断使用 `if/else`关键字

```logstash
if {}

if {} else {}

if {} else if {} else {}
```


