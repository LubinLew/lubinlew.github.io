# Add a File To Your Installer

安装文件是安装程序最基本的功能。 了解如何使用 Windows Installer 将文件放置在磁盘上不仅可以确保未来的可维护性，而且还方便在以后构建补丁（如有必要）。

## Step 1: 定义目录结构

安装程序通常是将许多文件安装到磁盘上的几个位置。 为了提高 WiX 文件的可读性，最好先定义安装目录，然后再列出将安装的文件。 目录是使用 [<Directory>](https://wixtoolset.org/documentation/manual/v3/xsd/wix/directory.html) 元素定义的，它描述了您希望在目标计算机上看到的文件夹层次结构。 以下示例定义了一个用于安装主应用程序可执行文件的目录。

```xml
<Directory Id="TARGETDIR" Name="SourceDir">
    <Directory Id="ProgramFilesFolder">
        <Directory Id="APPLICATIONROOTDIRECTORY" Name="My Application Name"/>
    </Directory>
</Directory>
```

Windows Installer 需要 `id` 为 [TARGETDIR](http://msdn.microsoft.com/library/aa372064.aspx) 的元素，它是安装的所有目录结构的根。 每个 WiX 项目都会有这个目录元素。

 第二个元素是 `id` 为  [ProgramFilesFolder](http://msdn.microsoft.com/library/aa370881.aspx) 的元素，使用预定义的 Windows Installer 属性来引用用户计算机上的 Program Files 文件夹。 在大多数情况下，这将解析为 `C:\Program Files\`。 

第三个目录元素的作用是在 `Program Files` 下创建您的应用程序文件夹，其`id` 为 `APPLICATIONROOTDIRECTORY` 以供以后在 WiX 项目中使用。 `id` 全部为大写字母，使其成为可以从 UI 或通过命令行设置的公共属性。

The result of these tags is a **c:\Program Files\My Application Name** folder on the target machine.

## Step 2: 将文件添加到安装程序包

使用两个元素将文件添加到安装程序：一个[<Component>](https://wixtoolset.org/documentation/manual/v3/xsd/wix/component.html) 元素指定安装的原子单元，一个  [<File>](https://wixtoolset.org/documentation/manual/v3/xsd/wix/file.html) 元素指定应该安装的文件。

 [<Component>](https://wixtoolset.org/documentation/manual/v3/xsd/wix/component.html) 元素元素描述了需要作为一个分类(自定义)安装的一组资源（通常是文件、注册表项和快捷方式）。 这与项目集是否包含用户可以选择安装的逻辑功能是分开的，这在第 3 步中讨论过。虽然当您第一次编写安装程序时，这似乎不是什么大问题，但组件在以下情况下发挥着关键作用 您决定在以后构建补丁。**通常，您应该限制每个组件只有一个文件**。Windows Installer 在单个安装程序中支持数千个组件，因此除非您有充分的理由，否则每个组件都应只有一个文件。 **每个组件都必须有自己唯一的 GUID**。不遵守这两个基本规则可能会导致后续维护过程中出现许多问题。

以下示例使用步骤 1 中定义的目录结构来安装两个文件：一个应用程序可执行文件和一个文档文件。

```xml
<DirectoryRef Id="APPLICATIONROOTDIRECTORY">
    <Component Id="myapplication.exe" Guid="PUT-GUID-HERE">
        <File Id="myapplication.exe" Source="MySourceFiles\MyApplication.exe" KeyPath="yes" Checksum="yes"/>
    </Component>
    <Component Id="documentation.html" Guid="PUT-GUID-HERE">
        <File Id="documentation.html" Source="MySourceFiles\documentation.html" KeyPath="yes"/>
    </Component>
</DirectoryRef>
```

[<DirectoryRef>](https://wixtoolset.org/documentation/manual/v3/xsd/wix/directoryref.html) 元素用于引用 `Step 1` 中的目录结构，所以文件会被安装到路径`c:\program files\My Application Name`文件夹下. 在 DirectoryRef 下面是两个 Component 元素, 每个Component元素都是一个需要被安装的文件. 每个 Component 元素都有一个 Id 和一个 Guid属性。Id 属性用于后续WiX项目中引用这个Component. Guid 用于稍后补丁，并且对于每个组件必须是唯一的. For information on generating GUIDs see [How To: Generate a GUID](https://wixtoolset.org/documentation/manual/v3/howtos/general/generate_guids.html).

每个组件下面是一个 File 元素，它执行将源文件打包到安装程序中的实际工作。 Id 用于引用 WiX 项目中其他位置的文件。 Source 属性指定文件在您机器上的位置，因此 WiX 可以找到它并将其构建到安装程序中。

KeyPath 属性设置为 yes 以告诉 Windows 安装程序应该使用此特定文件来确定是否安装了该组件。 如果不设置 KeyPath 属性，WiX 会按顺序查看组件下的子元素，并尝试自动选择其中之一作为关键路径。 允许 WiX 自动选择关键路径可能很危险，因为在组件下添加或删除子元素可能会无意中导致关键路径发生变化，从而导致安装问题。 通常，您应该始终将 KeyPath 属性设置为 yes，以确保在您将来更新设置创作时不会无意中更改keyPath。

对于在文件头中具有校验和值的可执行文件（这通常适用于所有可执行文件），Checksum 属性应设置为 yes，Windows 安装程序使用它来验证重新安装时文件的有效性。

## Step 3: 让 Windows Installer 安装文件

定义目录结构并列出要打包到安装程序中的文件后，最后一步是告诉 Windows Installer 实际安装文件。  [<Feature>](https://wixtoolset.org/documentation/manual/v3/xsd/wix/feature.html) 元素用于执行此操作，您可以在其中将安装程序分解为用户可以独立安装的逻辑部分。 以下示例创建了一个功能，用于安装步骤 2 中的应用程序可执行文件和文档。

```xml
<Feature Id="MainApplication" Title="Main Application" Level="1">
    <ComponentRef Id="myapplication.exe" />
    <ComponentRef Id="documentation.html" />
</Feature>
```

Feature 被赋予一个 ID。 如果您使用包含功能选择的安装程序 UI 序列，则 Title 属性包含在 UI 中为该功能显示的文本。 Level 属性应设置为 1 以默认启用该功能的安装。

 [<ComponentRef>](https://wixtoolset.org/documentation/manual/v3/xsd/wix/componentref.html) element 用于通过 Id 属性引用在步骤 2 中创建的组件。

## 完整的例子

以下是使用上述概念的完整示例。 此示例可以插入到 WiX 项目并编译，或从命令行编译和链接，以生成安装程序。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">
    <Product Id="*" UpgradeCode="PUT-GUID-HERE" Version="1.0.0.0" Language="1033" Name="My Application Name" Manufacturer="My Manufacturer Name">
        <Package InstallerVersion="300" Compressed="yes"/>
        <Media Id="1" Cabinet="myapplication.cab" EmbedCab="yes" />

        <!-- Step 1: Define the directory structure -->
        <Directory Id="TARGETDIR" Name="SourceDir">
            <Directory Id="ProgramFilesFolder">
                <Directory Id="APPLICATIONROOTDIRECTORY" Name="My Application Name"/>
            </Directory>
        </Directory>

        <!-- Step 2: Add files to your installer package -->
        <DirectoryRef Id="APPLICATIONROOTDIRECTORY">
            <Component Id="myapplication.exe" Guid="PUT-GUID-HERE">
                <File Id="myapplication.exe" Source="MySourceFiles\MyApplication.exe" KeyPath="yes" Checksum="yes"/>
            </Component>
            <Component Id="documentation.html" Guid="PUT-GUID-HERE">
                <File Id="documentation.html" Source="MySourceFiles\documentation.html" KeyPath="yes"/>
            </Component>
        </DirectoryRef>

        <!-- Step 3: Tell WiX to install the files -->
        <Feature Id="MainApplication" Title="Main Application" Level="1">
            <ComponentRef Id="myapplication.exe" />
            <ComponentRef Id="documentation.html" />
        </Feature>
    </Product>
</Wix>
```


