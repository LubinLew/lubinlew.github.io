# 账户信息收集

## 使用命令收集

```bash
wmic useraccount list full
```

使用 powershell 转换成 json 输出

```ps
$accounts = $(wmic useraccount list full)

$array=@()
$hash=@{}

for ($i=0;$i -lt $accounts.count;$i++) {
    $line = $accounts[$i].Replace("`r`n","").Trim()
    If(![String]::IsNullOrEmpty($line)) {
        $keypair = @(($accounts[$i]).Split('=').Trim())
        $hash[$keypair[0]] = $keypair[1]
        if ($keypair[0] -eq "Status") {
            $hash.Remove("Description")
            $account = ConvertTo-Json -InputObject $hash -Compress
            $array += $account
            $hash.Clear()
        }
    }
}

$output = "{`"navsec_account_info`": ["
foreach ($element in $array) {
    $account = ConvertFrom-Json -InputObject $element
    $output = -join($output, $(ConvertTo-Json -InputObject $account -Compress))
    $output = -join($output, ",")
}

$output = -join($output.Trim(","), "]}")
echo $output
```

## 使用 C++ 收集

调用函数 [NetUserEnum()](https://docs.microsoft.com/en-us/windows/win32/api/lmaccess/nf-lmaccess-netuserenum) 

----

## 参考链接

https://blog.csdn.net/y_m_h/article/details/104366470