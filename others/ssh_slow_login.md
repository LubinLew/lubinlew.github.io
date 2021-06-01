# SSH登录需要很久

使用root登录时需要很久才能登录,其他用户正常,
看了一下 `/var/log/messages` 发现下面错误信息

```bash
May 30 03:09:01 host systemd: Started Session 20785 of user root.
May 30 03:09:01 host systemd-logind: Failed to save session data /run/systemd/sessions/20785: File exists
May 30 03:09:01 host systemd-logind: Failed to save user data /run/systemd/users/0: File exists
May 30 03:09:26 host systemd-logind: Failed to save session data /run/systemd/sessions/20785: File exists
May 30 03:09:26 host systemd-logind: Failed to save user data /run/systemd/users/0: File exists
May 30 03:09:26 host systemd-logind: Failed to save user data /run/systemd/users/0: File exists
May 30 03:10:01 host systemd-logind: Failed to save session data /run/systemd/sessions/20786: File exists
May 30 03:10:01 host systemd-logind: Failed to save user data /run/systemd/users/0: File exists
```

### 解决方案

> https://www.explinux.com/2020/11/rhel-7-slow-login-delay-in-ssh-login.html


```bash
# 修改文件夹权限
chmod -R 0755 /run/user
chmod -R 0755 /run/systemd/users
chmod -R 0755 /run/systemd/sessions

systemctl daemon-reload

# 检查 dbus 和 polkit 服务, 如果没有启动则启动
systemctl status dbus.service
systemctl status polkit.service 

# 重启登录服务
systemctl restart systemd-logind

```