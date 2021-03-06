# 配置

ModSecurity 的配置分为 2 部分(引擎 和 规则), 总共4个配置文件。详细配置的方法在文件中都有详细的注释。

---

## 规则配置

 CRS规则配置共有3个文件，其中 900 和 999 是用户自定义和修改原规则的配置文件, 官方不推荐直接修改规则文件, 推荐使用这2个文件对现有规则进行配置。

[coreruleset/crs-setup.conf](modsecurity/crs/crs-setup.conf.md)

[coreruleset/rules/REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf](modsecurity/crs/REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.md)

[coreruleset/rules/RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf](modsecurity/crs/RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.md)

---

## 引擎配置

引擎即(libModSecurity)只有下面一个配置

[ModSecurity/modsecurity.conf](modsecurity/modsecurity/modsecurity.conf.md)

需要在该文件后面 `Include` CRS规则文件, 才能加载规则，否则只是启动引擎。

```bash
echo "Include coreruleset-3.3.0/crs-setup.conf" >> modsecurity.conf
echo "Include coreruleset-3.3.0/rules/*.conf"   >> modsecurity.conf
```

----

## Nginx配置示例

```nginx
server {
    listen 80 reuseport;

    modsecurity on;
    modsecurity_rules_file modsecurity/modsecurity.conf;

    location / {
        root html;
        index index.html;
    }
}
```
