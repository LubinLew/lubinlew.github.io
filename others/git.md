# GIT命令的使用

## 基本设置

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

#### 增加Branch

#### 删除Branch

```bash
# 删除本地分支
git branch -d <name>
# 删除远程分支
git push origin --delete <name>
```

### Tag 操作

#### 查看tag

```bash
git tag -l
```



#### 删除所有tag/release

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
