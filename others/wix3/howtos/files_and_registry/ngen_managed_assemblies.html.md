# How To: NGen Managed Assemblies During Installation

<a target="_blank" href="http://msdn.microsoft.com/en-us/magazine/cc163808.aspx">NGen</a> during installation can improve your managed application&apos;s startup time by creating native images of the managed assemblies on the target machine. This how to describes using the WiX support to NGen managed assemblies at install time.

## Step 1: Add the WiX .NET extensions library to your project

The WiX support for NGen is included in a WiX extension library that must be added to your project prior to use. If you are using WiX on the command-line you need to add the following to your candle and light command lines:

    -ext WixNetFxExtension

If you are using WiX in Visual Studio you can add the extensions using the Add Reference dialog:

1. Open your WiX project in Visual Studio
2. Right click on your project in Solution Explorer and select Add Reference...
3. Select the <strong>WixNetFxExtension.dll</strong> assembly from the list and click Add
4. Close the Add Reference dialog

## Step 2: Add the WiX .NET extensions namespace to your project

Once the library is added to your project you need to add the .NET extensions namespace to your project so you can access the appropriate WiX elements. To do this modify the top-level [&lt;Wix&gt;](../../xsd/wix/wix.html) element in your project by adding the following attribute:

```xml
xmlns:netfx="http://schemas.microsoft.com/wix/NetFxExtension"
```

添加了标准命名空间和 .NET 扩展命名空间的完整 Wix 元素如下所示：

```xml
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi"
     xmlns:netfx="http://schemas.microsoft.com/wix/NetFxExtension">
```


## Step 3: Mark the managed files for NGen

将 .NET 扩展库和命名空间添加到项目后，您可以使用 <NetFx:NativeImage> 元素在托管程序集上启用 NGen。 NativeImage 元素位于父 File 元素中：

```xml
<Component Id="myapplication.exe" Guid="PUT-GUID-HERE">
    <File Id="myapplication.exe" Source="MySourceFiles\MyApplication.exe" KeyPath="yes" Checksum="yes">
        <netfx:NativeImage Id="ngen_MyApplication.exe" Platform="32bit" Priority="0" AppBaseDirectory="APPLICATIONROOTDIRECTORY"/>
    </File>
</Component>
```

Id 属性是本机图像的唯一标识符。 Platform 属性指定应为其生成本机映像的平台，在本例中为 32 位。 Priority 属性指定应该何时生成图像，在这种情况下，是在设置过程中立即生成。 AppBaseDirectory 属性标识用于在映像生成期间搜索依赖程序集的目录。 在这种情况下，它被设置为应用程序的安装目录。