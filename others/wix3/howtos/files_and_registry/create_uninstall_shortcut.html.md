# 创建一个卸载快捷方式

安装应用程序时，通常需要在用户的“开始”菜单上提供卸载应用程序的快捷方式。 
本文演示了如何在开始菜单上创建卸载快捷方式所需的步骤。



## Step 1: 添加卸载快捷方式

<Shortcut> 元素用于向开始菜单添加卸载快捷方式，快捷方式指向 msiexec.exe（用于实际调用卸载过程的 Windows Installer 可执行文件）。 
在现有 ApplicationShortcut 组件中的任何地方添加以下内容：

```xml
<Shortcut Id="UninstallProduct"             
          Name="Uninstall My Application"
          Target="[SystemFolder]msiexec.exe"
          Arguments="/x [ProductCode]"
          Description="Uninstalls My Application" />
```

Target 属性指向 msiexec.exe 的位置。 Windows Installer SystemFolder 属性将解析为 msiexec.exe 所在的 System32 目录。 Arguments 属性用于通过传入安装包的 ProductCode 来让 msiexec.exe 知道要卸载哪个产品。

为了避免构建时的 ICE 验证错误，将 Shortcut 元素与注册表项和 RemoteFolder 元素结合起来很重要。 



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

        <DirectoryRef Id="ApplicationProgramsFolder">
            <Component Id="ApplicationShortcut" Guid="PUT-GUID-HERE">
                <Shortcut Id="ApplicationStartMenuShortcut" 
                     Name="My Application Name" 
                   Description="My Application Description"
                    Target="[#myapplication.exe]"
                          WorkingDirectory="APPLICATIONROOTDIRECTORY"/>
                <!-- Step 1: Add the uninstall shortcut to your installer package -->
                <Shortcut Id="UninstallProduct"             
                          Name="Uninstall My Application"
                          Description="Uninstalls My Application"
                          Target="[System64Folder]msiexec.exe"
                          Arguments="/x [ProductCode]"/>
                <RemoveFolder Id="ApplicationProgramsFolder" On="uninstall"/>
                <RegistryValue Root="HKCU" Key="Software\MyCompany\MyApplicationName" Name="installed" Type="integer" Value="1" KeyPath="yes"/>
           </Component>
        </DirectoryRef>

        <Feature Id="MainApplication" Title="Main Application" Level="1">
            <ComponentRef Id="myapplication.exe" />
            <ComponentRef Id="documentation.html" />
            <ComponentRef Id="ApplicationShortcut" />   
        </Feature>
    </Product>
</Wix>
```
