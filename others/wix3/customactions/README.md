# Standard Custom Actions

The WiX toolset contains several custom actions to handle configuring resources such as Internet Information Services web sites and virtual directories, SQL Server databases and scripts, user accounts, file shares, and more. These custom actions are provided in WiX extensions.

To get started using standard custom actions, see the [Using Standard Custom Actions](using_standard_customactions.html.md) topic.

For information about specific types of standard custom actions, see the following topics:

* [FileShare custom action](others/wix3/xsd/util/fileshare.html.md) (located in WixUtilExtension) - create and configure file shares.
* [Internet shortcut custom action](others/wix3/xsd/util/internetshortcut.html.md) (located in WixUtilExtension) - create shortcuts that point to Web sites.
* [OSInfo custom actions](osinfo.html.md) (located in WixUtilExtension) - set properties for OS information and standard directories that are not provided by default by Windows Installer.
* [Performance Counter custom action](perfmon.html.md) (located in WixUtilExtension) - install and uninstall performance counters.
* [Quiet Execution custom action](qtexec.html.md) (located in WixUtilExtension) - launch console executables without displaying a window.
* [Secure Objects custom action](others/wix3/xsd/util/permissionex.html.md) (located in WixUtilExtension) - secure (using ACLs) objects that the <a href='http://msdn.microsoft.com/library/aa369774.aspx' target="_blank">LockPermissions table</a> cannot.
* [Service Configuration custom action](others/wix3/xsd/util/serviceconfig.html.md) (located in WixUtilExtension) - configure attributes of a Windows service that the <a href='http://msdn.microsoft.com/library/aa371637.aspx' target="_blank">ServiceInstall table</a> cannot.
* [ShellExecute custom action](shellexec.html.md) (located in WixUtilExtension) - launch document or URL targets via the Windows shell.
* [User custom actions](others/wix3/xsd/util/user.html.md) (located in WixUtilExtension) - create and configure new users.
* [WixDirectXExtension](wixdirectxextension.html.md) - custom action that can be used to check the DirectX capabilities of the video card on the system.
* [WixExitEarlyWithSuccess](wixexitearlywithsuccess.html.md) (located in WixUtilExtension) - custom action that can be used to exit setup without installing the product. This can be useful in some major upgrade scenarios.
* [WixFailWhenDeferred](wixfailwhendeferred.html.md) (located in WixUtilExtension) - custom action that can be used to simulate installation failures to test rollback scenarios.
* [WixFirewallExtension](others/wix3/xsd/firewall/index.html.md) - Firewall custom action that can be used to add exceptions to the Windows Firewall.
* [WixGamingExtension](wixgamingextension.html.md) - Gaming custom action that can be used to add icons and tasks to Windows Game Explorer.
* [WixIIsExtension](others/wix3/xsd/iis/index.html.md) - Internet Information Services (IIS) custom actions that can be used to create and configure web sites, virtual directories, web applications, etc.
* [WixNetFxExtension](wixnetfxextension.html.md) - custom action to generate native code for .NET assemblies; properties to detect .NET Framework install state and service pack levels.
* [WixSqlExtension](others/wix3/xsd/sql/index.html.md) - SQL Server custom actions that can be used to create databases and execute SQL scripts and statements.
* [WixVSExtension](wixvsextension.html.md) - custom action to register help collections and Visual Studio packages; properties to detect install state and service pack levels for various Visual Studio editions.
* [XmlFile custom action](others/wix3/xsd/util/xmlfile.html.md) (located in WixUtilExtension) - configure and modify XML files as part of your installation package.
