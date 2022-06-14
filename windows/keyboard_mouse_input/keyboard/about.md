# 关于键盘输入

应用程序应该接受来自用户的键盘和鼠标输入。应用程序以发布到其窗口的消息的形式接收键盘输入。

本节涵盖以下主题：



## 键盘输入模型

系统通过安装适用于当前键盘的设备驱动程序为应用程序提供**与设备无关**的键盘支持。系统通过使用用户或应用程序当前选择的特定于语言的键盘布局来提供**与语言无关**的键盘支持。 键盘设备驱动程序从键盘接收扫描代码，这些代码被发送到键盘处理层( keyboard layout)，在那里它们被翻译成消息并发布到应用程序中的适当窗口。

分配给键盘上的每个键的是一个称为扫描码(*scan code*)的唯一值，它是键盘上键的设备相关标识符。当用户键入一个键时，键盘会生成两个扫描码:一个是在用户按下键时，另一个是在用户释放键时。

键盘设备驱动程序解释扫描码(*scan code*)并将其转换（映射）为虚拟键代码(*virtual-key code*)，这是由系统定义的与设备无关的值，用于识别键的用途。 翻译*扫描码*后，键盘处理层会创建一条消息，其中包含*扫描码*、*虚拟键码*和其他有关击键的信息，然后将消息放入系统消息队列中。 系统从系统消息队列中删除消息并将其发布到相应线程的消息队列中。 最终，线程的消息循环删除消息并将其传递给适当的窗口过程进行处理。下图说明了键盘输入模型。

![1](images/csinp-01.png)

---

## 键盘焦点和激活

系统将键盘消息发送到有键盘焦点(*keyboard focus*)的窗口的前台(foreground)线程的消息队列中。*键盘焦点*是窗口的临时属性。系统通用户的选择将键盘焦点从一个窗口转移到另一个窗口。显示器上的所有窗口之间共享键盘, 具有*键盘焦点*的窗口（从创建它的线程的消息队列中）接收所有键盘消息，直到焦点更改为不同的窗口。

线程可以调用 [GetFocus()](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getfocus) 函数来确定它的哪个窗口（如果有）当前具有键盘焦点。线程可以通过调用 [SetFocus()](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setfocus) 函数将键盘焦点赋予它的一个窗口。当键盘焦点从一个窗口切换到另一个窗口时，系统会向失去焦点的窗口发送 [WM_KILLFOCUS](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-killfocus) 消息，然后向获得焦点的窗口发送 [WM_SETFOCUS](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-setfocus) 消息。

键盘焦点的概念与活动窗口(active window)的概念有关。活动窗口是用户当前正在使用的顶级窗口。具有键盘焦点的窗口要么是活动窗口，要么是活动窗口的子窗口。为了帮助用户识别活动窗口，系统将其放在 Z 顺序的顶部并突出显示其标题栏（如果有）和边框。

用户可以通过单击顶层窗口、使用 ALT+TAB 或 ALT+ESC 组合键选择它或从任务列表中选择它来激活顶层窗口。线程可以使用 [SetActiveWindow()](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setactivewindow) 函数激活顶级窗口。它可以通过 [GetActiveWindow()](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getactivewindow) 函数判断它创建的顶层窗口是否处于活动状态。

当一个窗口被停用而另一个窗口被激活时，系统会发送 [WM_ACTIVATE](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-activate) 消息。如果窗口被停用，`wParam` 参数的低位字为零，如果窗口被激活，则为非零。当默认窗口过程(default window procedure)接收到 [WM_ACTIVATE](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-activate) 消息时，它会将键盘焦点设置到活动窗口。

要阻止键盘和鼠标输入事件到达应用程序，请使用 [BlockInput()](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-blockinput)。注意，[BlockInput()](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-blockinput) 函数不会干扰异步键盘输入状态表(asynchronous keyboard input-state table)。这意味着在输入被阻塞时调用 [SendInput()](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput) 函数将更改异步键盘输入状态表。

---

## 按键消息

按下一个键会产生一个 [WM_KEYDOWN](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown) 或 [WM_SYSKEYDOWN](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown) 消息, 这个消息被发送到具有键盘焦点的窗口的线程消息队列中。 

释放一个键会导致 [WM_KEYUP](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keyup) 或 [WM_SYSKEYUP](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-syskeyup) 消息被放置在队列中。

按键压下和按键释放消息通常成对出现，但如果用户按住某个键的时间足够长以启动键盘的自动重复功能，系统会连续生成许多 [WM_KEYDOWN](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown) 或 [WM_SYSKEYDOWN](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown) 消息。 然后，当用户释放密钥时，它会生成单个[WM_KEYUP](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keyup) 或 [WM_SYSKEYUP](https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-syskeyup) 消息。

本节涵盖以下主题：

- [系统和非系统按键](#系统和非系统按键)
- [虚拟键代码](#虚拟键代码)
- [击键消息标志](#击键消息标志)

### 系统和非系统按键

系统区分系统击键和非系统击键。系统击键产生系统击键消息，WM_SYSKEYDOWN 和 WM_SYSKEYUP。非系统击键产生非系统击键消息，WM_KEYDOWN 和 WM_KEYUP。

如果您的窗口过程必须处理系统击键消息，请确保在处理完消息后，过程将其传递给 [DefWindowProc](https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-defwindowproca) 函数。否则，只要窗口具有键盘焦点，所有涉及 ALT 键的系统操作都将被禁用。也就是说，用户将无法访问窗口的菜单或系统菜单，或者使用 ALT+ESC 或 ALT+TAB 组合键来激活不同的窗口。

系统击键消息主要供系统使用，而不是由应用程序使用。系统使用它们为菜单提供内置键盘界面，并允许用户控制哪个窗口处于活动状态。当用户键入一个键与 ALT 键组合时，或者当用户键入并且没有窗口具有键盘焦点时（例如，当活动应用程序被最小化时），系统击键消息会生成。在这种情况下，消息被发布到附加到活动窗口的消息队列中。

非系统按键消息供应用程序窗口使用； DefWindowProc 函数对它们没有任何作用。窗口过程可以丢弃它不需要的任何非系统击键消息。

### 虚拟键代码

>  虚拟键代码(Virtual-Key Codes)

击键消息的 `wParam` 参数包含被按下或释放的键的虚拟键代码。 窗口过程根据虚拟键代码的值处理或忽略击键消息。

典型的窗口过程只处理它接收到的击键消息的一小部分，而忽略其余部分。 例如，一个窗口过程可能只处理 WM_KEYDOWN 键击消息，并且只处理那些包含光标移动键、shift 键（也称为控制键）和功能键的虚拟键代码的消息。 典型的窗口过程不处理来自字符键的击键消息。 相反，它使用 TranslateMessage 函数将消息转换为字符消息。 有关 TranslateMessage 和字符消息的更多信息，请参阅[字符消息](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#character-messages)。

### 击键消息标志

击键消息的 `lParam` 参数包含有关生成消息的击键的附加信息。该信息包括重复计数、扫描码、扩展键标志、上下文代码、前一个键状态标志和转换状态标志。 下图显示了这些标志和值在 `lParam` 参数中的位置。

![2](images/csinp-02.png)

应用程序可以使用以下值从 `lParam` 的高位字中获取击键标志。

| Value                     | Description                                                                                                                                      |
| ------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| **KF_EXTENDED**<br>0x0100 | Manipulates the [extended key flag](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#extended-key-flag).             |
| **KF_DLGMODE**<br>0x0800  | Manipulates the dialog mode flag, which indicates whether a dialog box is active.                                                                |
| **KF_MENUMODE**<br>0x1000 | Manipulates the menu mode flag, which indicates whether a menu is active.                                                                        |
| **KF_ALTDOWN**<br>0x2000  | Manipulates the [context code flag](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#context-code).                  |
| **KF_REPEAT**<br>0x4000   | Manipulates the [previous key state flag](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#previous-key-state-flag). |
| **KF_UP**<br>0x8000       | Manipulates the [transition state flag](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#previous-key-state-flag).   |

示例代码

```c
case WM_KEYDOWN:
case WM_KEYUP:
case WM_SYSKEYDOWN:
case WM_SYSKEYUP:
{
    WORD vkCode = LOWORD(wParam);                                 // virtual-key code
    
    WORD keyFlags = HIWORD(lParam);

    WORD scanCode = LOBYTE(keyFlags);                             // scan code
    BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED; // extended-key flag, 1 if scancode has 0xE0 prefix
    
    if (isExtendedKey)
        scanCode = MAKEWORD(scanCode, 0xE0);

    BOOL repeatFlag = (keyFlags & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat
    WORD repeatCount = LOWORD(lParam);                            // repeat count, > 0 if several keydown messages was combined into one message

    BOOL upFlag = (keyFlags & KF_UP) == KF_UP;                    // transition-state flag, 1 on keyup

    // if we want to distinguish these keys:
    switch (vkCode)
    {
    case VK_SHIFT:   // converts to VK_LSHIFT or VK_RSHIFT
    case VK_CONTROL: // converts to VK_LCONTROL or VK_RCONTROL
    case VK_MENU:    // converts to VK_LMENU or VK_RMENU
        vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
        break;
    }

    // ...
}
break;
```

#### 重复计数

您可以检查重复计数以确定击键消息是否代表多次击键。 当键盘生成 WM_KEYDOWN 或 WM_SYSKEYDOWN 消息的速度快于应用程序处理它们的速度时，系统会增加计数。 当用户按住一个键足够长的时间以启动键盘的自动重复功能时，通常会发生这种情况。 系统不是用生成的按键消息填充系统消息队列，而是将消息组合成单个按键消息并增加重复计数。 释放键无法启动自动重复功能，因此 WM_KEYUP 和 WM_SYSKEYUP 消息的重复计数始终设置为 1。

#### 扫描码

扫描码(scan code)是用户按键时键盘硬件生成的值。它是一个与设备相关的值，用于标识按下的键，而不是键所代表的字符。应用程序通常会忽略扫描码。 相反，它使用独立于设备的虚拟键代码来解释击键消息。

#### 扩展键标志

扩展键标志指示击键消息是否源自增强型 101/102-键键盘上的附加键之一。 如果指定，扫描码由两个字节的序列组成，其中第一个字节的值为 0xE0。

下面是扩展键列表:

| 按键               | vcode |
| ---------------- | ----- |
| R-ALT            |       |
| R-CTRL           |       |
| INS              |       |
| DEL              |       |
| HOME             |       |
| END              |       |
| Page Up          |       |
| Page Down        |       |
| Pause Break      |       |
| PrintScreen      |       |
| 数字小键盘上的 Num Lock |       |
| 数字小键盘上的 方向键(4个)  |       |
| 数字小键盘上的 除号键      |       |
| 数字小键盘上的 回车键      |       |

> 右手的 SHIFT 键不被视为扩展键，它有一个单独的扫描码。

#### 

#### 上下文代码

上下文代码(context code)指示生成击键消息时 ALT 键是否按下。 如果 ALT 键按下，则代码为 1，否则代码为 0。



#### Previous Key-State Flag

先前的键状态标志指示生成击键消息的键先前是向上还是向下。 如果该键先前已关闭，则为 1，如果该键先前已打开，则为 0。 您可以使用此标志来识别由键盘的自动重复功能生成的击键消息。 对于由自动重复功能生成的 WM_KEYDOWN 和 WM_SYSKEYDOWN 击键消息，此标志设置为 1。 对于 WM_KEYUP 和 WM_SYSKEYUP 消息，它始终设置为 1。



#### 转换状态标志

转换状态标志指示是按下键还是释放键生成击键消息。对于 WM_KEYDOWN 和 WM_SYSKEYDOWN 消息，此标志始终设置为 0； 对于 WM_KEYUP 和 WM_SYSKEYUP 消息，它始终设置为 1。

---

## 字符信息

[About Keyboard Input - Win32 apps | Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#character-messages)


