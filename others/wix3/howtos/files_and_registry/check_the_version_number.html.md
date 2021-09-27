# 安装过程中检查版本号

在安装过程中, 经常需要查找硬盘上某个文件的版本号。该检查通常在稍后安装的条件语句(Condition)之前使用。
例如当某些文件不存在时阻止用户安装， 或者根据文件版本是否足够高来显示自定义安装 UI。
本文会演示如何验证磁盘上文件的版本，如果文件版本低于预期，则使用结果[<Property>](others/wix3/xsd/wix/property.html.md)属性阻止应用程序的安装。



## Step 1: 决定文件的版本号

文件版本是使用 [<Property>](others/wix3/xsd/wix/property.html.md), [<DirectorySearch>](others/wix3/xsd/wix/directorysearch.html.md) 和 [<FileSearch>](others/wix3/xsd/wix/filesearch.html.md) 元素来确定的。
以下示例代码演示的是在机器的 System32 目录中查找 user32.dll 文件，并检查它是否至少为 6.0.6001.1751 版本。

```xml
<Property Id="USER32VERSION">
    <DirectorySearch Id="SystemFolderDriverVersion" Path="[SystemFolder]">
        <FileSearch Name="user32.dll" MinVersion="6.0.6001.1750"/>
    </DirectorySearch>
</Property>
```

搜索文件是通过描述要搜索的目录来完成的，然后指定要在该目录中查找的文件。
[<Property>](others/wix3/xsd/wix/property.html.md)元素定义文件搜索结果的 Id。此 ID 稍后在 WiX 项目中使用，例如在条件(Condition)中。
DirectorySearch 元素用于构建目录层次结构以搜索文件。

在这种情况下，它被赋予一个唯一的 ID，并且路径被设置为 Windows 安装程序定义的 [SystemFolder](http://msdn.microsoft.com/library/aa372055.aspx) 属性，该属性指向用户的 Windows\System32 目录。 
FileSearch 元素指定要在父 DirectorySearch 文件夹中查找的文件的名称。 MinVersion 属性指定要查找的文件的最低版本。

如果成功找到该文件，则 USER32VERSION 属性将设置为 user32.dll 文件的完整路径。

重要提示：对文件进行区域设置中立搜索时，必须将 MinVersion 属性设置为比要搜索的实际版本低一个修订号。 
在此示例中，虽然我们要查找文件版本为 6.0.6001.1751，但 MinVersion 必须设置为 6.0.6001.1750。 
这是因为 Windows Installer 匹配文件版本的方式存在一个怪癖，详见[此链接](http://msdn.microsoft.com/library/aa371853.aspx)。

## Step 2: 在 Condition 中使用该属性(property)

一旦确定存在请求版本的文件，那么您就可以在条件中使用该属性。
下面是一个简单的例子，如果 user32.dll 文件版本太低，它会阻止应用程序的安装。

```xml
<Condition Message="The installed version of user32.dll is not high enough to support this installer.">
    <![CDATA[Installed OR USER32VERSION]]>
</Condition>
```

[Installed](http://msdn.microsoft.com/library/aa369297.aspx) 是 Windows Installer 属性，可确保仅在用户安装应用程序时进行检查，而不是在修复或删除时进行。 如果USER32VERSION属性设置不为空， 则将通过，如果未设置，则将失败。 如果找到具有适当文件版本的 user32.dll 文件，则步骤 1 中的文件检查会将属性设置为该文件的完整路径，否则不会设置它。
