# Controller-USR
 
先在gituhub上新建一个仓库

1、在本地建一个git目录

#git config --global user.name "764889950"

#git config --global user.email "764889950@qq.com"

#git init

查看 global 配置

#git config --global --list


2、创建ssh密匙

#ssh-keygen -t rsa -C "764889950@qq.com"



使用浏览器到git仓库，进入Account Settings，左边选择SSH Keys添加秘钥（密钥位于~/.ssh/id_rsa.pub）

3、验证是否成功，在git bash下输入

#ssh -T git@github.com


You’ve successfully authenticated, but GitHub does not provide shell access 。这就表示已成功连上github。

#git@github.com: Permission denied (publickey).

不通就是

#ssh-agent -s

#ssh-add ~/.ssh/id_rsa 操作这两步



添加远程地址

#git remote add USR git@github.com:764889950/USR.git


将项目所有文件添加到本地仓库中

#git add .

#git commit -m "init"   #添加文件描述


进行push前先将远程仓库pull到本地仓库（fatal: refusing to merge unrelated histories 表示pull的内容是不同的分支）

#git pull USR master



强制push本地仓库到远程(会覆盖远程仓库)

#git push -u USR master -f


下载克隆源码(上级目录，并且删除USR中的内容)

git clone git@github.com:764889950/USR.git
