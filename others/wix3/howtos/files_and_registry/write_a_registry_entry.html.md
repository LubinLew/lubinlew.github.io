# 安装过程中写注册表

在安装期间写入注册表项与在安装期间写入文件类似。 需要描述要写入的注册表层次结构，指定要创建的注册表值，然后将组件添加到您的功能列表中。

## Step 1: 描述注册表布局和值

下面的示例说明了如何将两个注册表项写入特定值，另一个写入默认值。

```xml
<DirectoryRef Id="TARGETDIR">
    <Component Id="RegistryEntries" Guid="PUT-GUID-HERE">
        <RegistryKey Root="HKCU"
                     Key="Software\MyCompany\MyApplicationName"
              Action="createAndRemoveOnUninstall">
            <RegistryValue Type="integer" Name="SomeIntegerValue" Value="1" KeyPath="yes"/>
            <RegistryValue Type="string" Value="Default Value"/>
        </RegistryKey>
    </Component>
</DirectoryRef>
```

该代码段以指向 Windows Installer 定义的 [TARGETDIR](http://msdn.microsoft.com/library/aa372064.aspx) 目录的 DirectoryRef 开头。这实际上意味着注册表项应该安装到目标用户的机器上。 
DirectoryRef 下是一个 Component 元素，它将要安装的注册表项组合在一起。 该组件被赋予一个 id 以供稍后在 WiX 项目中参考和一个唯一的 guid。

注册表项是通过首先使用 [<RegistryKey>](others/wix3/xsd/wix/registrykey.html.md) 元素来指定值在注册表中的位置创建的。
在此示例中，注册表项位于 `HKEY_CURRENT_USER\Software\MyCompany\MyApplicationName` 下。
可选的 Action 属性用于告诉 Windows Installer 应在安装时创建该键（如有必要），并且应在卸载时删除该键及其所有子值。

在 RegistryKey 元素下，<RegistryValue> 元素用于创建实际的注册表值。
第一个是名为 SomeIntegerValue 值，它是整数类型，值为 1。它也被标记为组件的 KeyPath，
Windows 安装程序使用它来确定该组件是否安装在机器上。 
第二个 RegistryValue 元素将键的默认值设置为 `Default Value` 的字符串值。

RegistryKey 和 RegistryValue 元素上的 id 属性被省略，因为不需要在 WiX 项目文件的其他地方引用这些项目。 WiX 将根据注册表项、值和父组件名称为元素自动生成 id。

## Step 2: 让 Windows Installer 安装条目

定义目录结构并列出要打包到安装程序中的注册表项后，最后一步是告诉 Windows Installer 实际安装注册表项。 
<Feature> 元素用于执行此操作。 以下代码段添加了对注册表项组件的引用，并且应插入到父 Feature 元素中。

```xml
<ComponentRef Id="RegistryEntries" />
```

The [<ComponentRef>](others/wix3/xsd/wix/componentref.html) 元素用于通过 Id 属性引用在步骤 1 中创建的组件。