# 使用github搭建个人博客

## 建立博客仓库

仓库的名称必须为 `{username}.github.io`



## 设置个人域名

`{username}.github.io`这个域名如果你觉得不好记, 那么可以自己购买一个域名然后指向这个 github域名。

### 购买域名并设置CNAME

首先在服务提供商处购买一个域名， 比如我在godaddy上花15块大洋购买的 `lubinlew.site`。然后在这个域名的DNS设置处，增加一个指向github域名的CNAME。
![cname-config](/_sources/images/others/blog/godaddy_cname_config.png)

配置github个人域名

在仓库的设置中配置 Github Pages 选项

![page-config](/_sources/images/others/blog/github_pages_custom_domain.png)
