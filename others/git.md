# GIT命令的使用

**目录**

<!-- TOC -->

- [GIT命令的使用](#git命令的使用)
  - [基本设置](#基本设置)
    - [设置用户名称和邮箱](#设置用户名称和邮箱)
    - [保存仓库的账号密码](#保存仓库的账号密码)
    - [颜色显示设置](#颜色显示设置)
      - [默认颜色](#默认颜色)
      - [具体设置](#具体设置)
  - [基本操作](#基本操作)
    - [Branch操作](#branch操作)
      - [查看branch](#查看branch)
      - [切换Branch](#切换branch)
      - [新建Branch](#新建branch)
      - [删除Branch](#删除branch)
      - [删除远程Branch](#删除远程branch)
      - [从Tag创建新的Brach](#从tag创建新的brach)
    - [TAG操作](#tag操作)
      - [查看tag](#查看tag)
      - [新建tag](#新建tag)
      - [删除tag](#删除tag)
        - [删除所有tag](#删除所有tag)
  - [进阶操作](#进阶操作)
    - [删除所有提交记录](#删除所有提交记录)
    - [同步fork项目](#同步fork项目)
    - [gitlab 同步 github 项目](#gitlab-同步-github-项目)

<!-- /TOC -->

* [基本设置](#基本设置)
  - [设置用户名称和邮箱](#设置用户名称和邮箱)
  - [保存仓库的账号密码](#保存仓库的账号密码)
  - [颜色显示设置](#颜色显示设置)
* [基本操作](#基本操作)
  - [Branch操作](#Branch操作)
    * [查看branch](#查看branch)
    * [切换Branch](#切换branch)
    * [新建Branch](#新建branch)
    * [删除Branch](#删除Branch)
  - [TAG操作](#TAG操作)
    * [查看tag](#查看tag)
    * [删除tag](#删除tag)
    * [新建tag](#新建tag)
* [进阶操作](#进阶操作)
  - [删除所有提交记录](#删除所有提交记录)
  - [同步fork项目](#同步fork项目)
  - [gitlab 同步 github 项目](#gitlab-同步-github-项目)

---

## 基本设置

### git clone 失败

```bash
fetch-pack: unexpected disconnect while reading sideband packet
fatal: early EOF
fatal: fetch-pack: invalid index-pack output
```

解决方法:

```bash
git config --global pack.threads "1"
git config --global pack.windowMemory   "100m"
git config --global pack.packSizeLimit  "100m"
git config --global pack.deltaCacheSize "512m"
```


### 设置用户名称和邮箱

```bash
# 配置全局
git config --global user.name "username"
git config --global user.email useremail@company.com

#配置当前仓库
git config --local user.name "username"
git config --local user.email useremail@company.com
```

### 保存仓库的账号密码

```bash
# 保存本地仓库的账号
git config --local credential.helper store
# 保存git全局账号
git config --global credential.helper store
```

### 颜色显示设置

#### 默认颜色

```bash
# 开启默认颜色设置
git config --global color.ui true
# 关闭默认颜色设置
git config --global color.ui false
```

#### 具体设置

详细配置见 [Git - git-config Documentation](https://git-scm.com/docs/git-config#_values)

## 基本操作

### Branch操作

#### 查看branch

```bash
# 查看该仓库本地已有的分支
git branch
# 查看该仓库的所有分支
git branch -a
```

#### 切换Branch

```bash
git checkout <name>
```

#### 新建Branch

```bash
git branch <name>
```

#### 删除Branch

```bash
# 删除本地分支
git branch -d <name>
# 删除远程分支
git push origin --delete <name>
```

#### 删除远程Branch

```bash
git push origin --delete branch_name
```

#### 从Tag创建新的Brach

```bash
git branch <new-branch-name> <tag-name>
```

----

### TAG操作

#### 查看tag

```bash
git tag -l
```

#### 新建tag

```bash
git tag -a <tag-name> -m <tag-message> <tag-version>

# 例子
git tag -a v0.1 -m "version 0.1 released push url" d5a65e9
```

#### 删除tag

```bash
# 删除本地tag
git tag -d <name>

# 删除远端tag
git push origin --delete <name>
```

##### 删除所有tag

```bash
# tag 列表
export TAGLIST=`git tag -l`
# 删除本地所有 tag
git tag -d $TAGLIST
# 删除 远端 所有 tag
echo $TAGLIST | xargs git push origin --delete
# 提交
git push origin --tags
```

----

## 进阶操作

### 删除所有提交记录

提交错误(包含敏感信息)等时候, 就需要清除提交日志

```bash
#切换分支
git checkout --orphan latest_branch

#添加所有文件
git add -A

#提交更改
git commit -am "no message"

#删除分支
git branch -D master

#重命名分支
git branch -m master

#强制更新
git push -f origin master
```

### 同步fork项目

> [configuring-a-remote-for-a-fork](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/configuring-a-remote-for-a-fork)
> 
> [syncing-a-fork](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/syncing-a-fork)

当我们Fork了一个项目后，原项目更新了，我们希望Fork的项目也更新到最新。

首先需要`git remove -v` 产看一下是否有原项目的upstream

```bash
$ git remote -v
origin  https://github.com/LubinLew/lua-nginx-module.git (fetch)
origin  https://github.com/LubinLew/lua-nginx-module.git (push)
upstream        https://github.com/openresty/lua-nginx-module.git (fetch)
upstream        https://github.com/openresty/lua-nginx-module.git (push)
```

如果没有需要添加一下

```bash
git remote add upstream https://github.com/openresty/lua-nginx-module.git
```

更新upstream到你的项目, 然后 merge 代码并提交

```bash
git fetch upstream
# 只下载某个tag
# git fetch upstream refs/tags/v4.2.1
git merge upstream/master
git push
```

### gitlab 同步 github 项目

将 github 上的更新同步到 本地 gitlab 库中。

社区版本的 gitlab 项目中的镜像设置不允许 pull 只允许 push， pull 需要商业授权，所以只能手动同步。

```bash
git clone git@gitlab.local.com:openvas/gsa.git
cd gsa
git remote add --mirror=fetch github https://github.com/greenbone/gsa.git
git pull --all
git push --all
```

只同步tag的情况

```bash
git fetch github --tags                 # 下载所有github上的所有 tag
git push origin refs/tags/v4.2.3        # 只推送 v4.2.3 版本到 gitlab上

```


