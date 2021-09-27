# 在开始菜单中创建快捷方式

安装应用程序时，通常需要在用户的 “开始” 菜单上放置快捷方式以提供程序的快速启动。 本文将演示如何在开始菜单上创建快捷方式。 



## Step 1: 定义目录结构

开始菜单快捷方式安装在与常规应用程序文件不同的目录中，因此需要修改安装程序的目录结构。 以下代码片段应放置在具有 TARGETDIR ID 的 <Directory> 元素下。

```xml
<Directory Id="ProgramMenuFolder">
    <Directory Id="ApplicationProgramsFolder" Name="My Application Name"/>
</Directory>
```

[ProgramMenuFolder](http://msdn.microsoft.com/library/aa370882.aspx) Id 是一个标准的 Windows Installer 属性，它指向目标机器上的开始菜单文件夹。 第二个 Directory 元素在“开始”菜单上创建一个名为“My Application Name”的子文件夹，并为其提供一个 ID，以便稍后在 WiX 项目中使用。


## Step 2: 将快捷方式添加到安装程序包

使用三个元素向安装程序添加一个快捷方式：

- 一个 <Component> 元素指定安装的原子单元，

- 一个 <Shortcut> 元素指定应该安装的快捷方式，

- 一个 <RemoveFolder> 元素以确保应用程序卸载时正确清理。

以下示例使用在步骤 1 中定义的目录结构来创建“开始”菜单快捷方式。

```xml
<DirectoryRef Id="ApplicationProgramsFolder">
    <Component Id="ApplicationShortcut" Guid="PUT-GUID-HERE">
        <Shortcut Id="ApplicationStartMenuShortcut" 
                  Name="My Application Name"
                  Description="My Application Description"
                  Target="[#myapplication.exe]"
                  WorkingDirectory="APPLICATIONROOTDIRECTORY"/>
        <RemoveFolder Id="CleanUpShortCut" Directory="ApplicationProgramsFolder" On="uninstall"/>
        <RegistryValue Root="HKCU" Key="Software\MyCompany\MyApplicationName" Name="installed" Type="integer" Value="1" KeyPath="yes"/>
    </Component>
</DirectoryRef>
```

<DirectoryRef> 元素用于引用在步骤 1 中创建的目录结构。通过引用 ApplicationProgramsFolder 目录，快捷方式将安装到用户的“开始”菜单中的“My Application Name”文件夹中。

DirectoryRef 下面是一个单独的组件，用于对用于安装快捷方式的元素进行分组。第一个元素是快捷方式，它在开始菜单中创建实际的快捷方式。 Id 属性是快捷方式的唯一 ID。 Name 属性是将显示在“开始”菜单中的文本。Description 是附加应用程序描述的可选属性。 Target 属性指向要在磁盘上启动的可执行文件。请注意它如何使用 [#FileId] 语法引用完整路径，其中 myapplication.exe 是先前定义的。 WorkingDirectory 属性设置快捷方式的工作目录。

要为快捷方式设置可选图标，您需要首先使用 <Icon> 元素在安装程序中包含该图标，然后使用 Shortcut 元素上的 Icon 属性引用它。

除了创建快捷方式之外，该组件还包含另外两个重要的部分。第一个是 RemoveFolder 元素，它确保在用户卸载应用程序时从“开始”菜单中正确删除 ApplicationProgramsFolder。第二个在安装时创建一个注册表项，指示应用程序已安装。这是必需的，因为在为当前用户安装非广告快捷方式时，快捷方式不能用作组件的 KeyPath。有关创建注册表项的详细信息，请参阅如何：在安装过程中编写注册表项。



## Step 3: 告诉 Windows Installer 安装快捷方式

定义目录结构并列出要打包到安装程序中的快捷方式后，最后一步是告诉 Windows Installer 实际安装快捷方式。 <Feature> 元素用于执行此操作。 以下代码段添加了对快捷方式组件的引用，并且应插入到父 Feature 元素中。

```xml
<ComponentRef Id="ApplicationShortcut" />
```

<ComponentRef> 元素用于通过 Id 属性引用在步骤 2 中创建的组件。

## 完整示例

以下是使用上述概念的完整示例。 此示例可以插入到 WiX 项目并编译，或从命令行编译和链接，以生成安装程序。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">
    <Product Id="*" UpgradeCode="PUT-GUID-HERE" Version="1.0.0.0" Language="1033" Name="My Application Name" Manufacturer="My Manufacturer Name">
        <Package InstallerVersion="300" Compressed="yes"/>
        <Media Id="1" Cabinet="myapplication.cab" EmbedCab="yes" />

        <Directory Id="TARGETDIR" Name="SourceDir">
            <Directory Id="ProgramFilesFolder">
                <Directory Id="APPLICATIONROOTDIRECTORY" Name="My Application Name"/>
            </Directory>
            <!-- Step 1: Define the directory structure -->
            <Directory Id="ProgramMenuFolder">
                <Directory Id="ApplicationProgramsFolder" Name="My Application Name"/>
            </Directory>
        </Directory>

        <DirectoryRef Id="APPLICATIONROOTDIRECTORY">
            <Component Id="myapplication.exe" Guid="PUT-GUID-HERE">
                <File Id="myapplication.exe" Source="MySourceFiles\MyApplication.exe" KeyPath="yes" Checksum="yes"/>
            </Component>
            <Component Id="documentation.html" Guid="PUT-GUID-HERE">
                <File Id="documentation.html" Source="MySourceFiles\documentation.html" KeyPath="yes"/>
            </Component>
        </DirectoryRef>

        <!-- Step 2: Add the shortcut to your installer package -->
        <DirectoryRef Id="ApplicationProgramsFolder">
            <Component Id="ApplicationShortcut" Guid="PUT-GUID-HERE">
                <Shortcut Id="ApplicationStartMenuShortcut" 
                     Name="My Application Name" 
                   Description="My Application Description"
                    Target="[#myapplication.exe]"
                          WorkingDirectory="APPLICATIONROOTDIRECTORY"/>
                <RemoveFolder Id="ApplicationProgramsFolder" On="uninstall"/>
                <RegistryValue Root="HKCU" Key="Software\MyCompany\MyApplicationName" Name="installed" Type="integer" Value="1" KeyPath="yes"/>
           </Component>
        </DirectoryRef>

        <Feature Id="MainApplication" Title="Main Application" Level="1">
            <ComponentRef Id="myapplication.exe" />
            <ComponentRef Id="documentation.html" />
            <!-- Step 3: Tell WiX to install the shortcut -->
            <ComponentRef Id="ApplicationShortcut" />   
        </Feature>
    </Product>
</Wix>
```