# 创建一个指向网页的快捷方式

作为安装过程的一部分，WiX 支持创建 Internet 站点的快捷方式。
本文将演示如何引用必要的实用程序库并将 Internet 快捷方式添加到您的安装程序。 



## Step 1: 将 WiX Utility 扩展库添加到您的项目

WiX 对 Internet 快捷方式的支持包含在 WiX 扩展库中，必须在使用前将其添加到您的项目中。
如果您在命令行上使用 WiX，则需要将以下内容添加到  candle 和 light 命令行中：

```bash
    -ext WiXUtilExtension
```

如果您在 Visual Studio 中使用 WiX，则可以使用“添加引用”对话框添加扩展：

1. 在 Visual Studio 中打开 ViX 项目
2. Right click on your project in Solution Explorer and select Add Reference...
3. Select the **WixUtilExtension.dll** assembly from the list and click Add
4. 关闭添加引用对话框



## Step 2: 将 WiX Utility 扩展命名空间添加到您的项目

将库添加到您的项目后，您需要将实用程序扩展命名空间添加到您的项目，以便您可以访问适当的 WiX 元素。
为此，请通过添加以下属性来修改项目中的顶级 <Wix> 元素：

```xml
xmlns:util="http://schemas.microsoft.com/wix/UtilExtension"
```

添加了标准命名空间和实用程序扩展命名空间的完整 Wix 元素如下所示：

```xml
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi"
     xmlns:util="http://schemas.microsoft.com/wix/UtilExtension">
```

## 

## Step 3: 将 Internet 快捷方式添加到您的安装程序包

Internet 快捷方式是使用 <Util:InternetShortcut> 元素创建的。 
以下示例将 InternetShortcut 元素添加到如何：在“开始”菜单上创建快捷方式中的现有快捷方式创建示例。

```xml
<DirectoryRef Id="ApplicationProgramsFolder">
    <Component Id="ApplicationShortcut" Guid="PUT-GUID-HERE">
        <Shortcut Id="ApplicationStartMenuShortcut" 
                  Name="My Application Name"
                  Description="My Application Description"
                  Target="[#MyApplicationExeFileId]"
                  WorkingDirectory="APPLICATIONROOTDIRECTORY"/>
        <util:InternetShortcut Id="OnlineDocumentationShortcut"
                        Name="My Online Documentation"
                               Target="http://wixtoolset.org/"/>
        <RemoveFolder Id="ApplicationProgramsFolder" On="uninstall"/>
        <RegistryValue Root="HKCU" Key="Software\MyCompany\MyApplicationName" Name="installed" Type="integer" Value="1" KeyPath="yes"/>
    </Component>
</DirectoryRef>
```

InternetShortcut 被赋予一个具有 Id 属性的唯一 ID。 在这种情况下，应用程序的开始菜单文件夹。 Name 属性指定开始菜单上快捷方式的名称。 Target 属性指定快捷方式的目标地址。 <DirectoryRef> 元素用于引用项目文件已经定义的目录结构。 通过引用 ApplicationProgramsFolder 目录，快捷方式将安装到用户的“开始”菜单中的“我的应用程序名称”文件夹中。