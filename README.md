# 我真是爱死C++了


void只表示函数返回值类型为空
- 常量
- 变量
- 函数

- 函数不能与全局变量重名
- 函数内的变量如果与函数重名，则服从作用域的约束。


### docker
```shell
删除所有停止的容器
sudo docker rm $(sudo docker ps -a -q)

删除所有无效镜像
sudo docker rmi $(sudo docker images -f "dangling=true" -q)

```