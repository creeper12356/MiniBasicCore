## **TODO list**
* (Ok)暂时不考虑表达式中有负数的情况
* (Ok)禁止tab输入
* 测试Expression构造函数的内存泄漏情况
* 实现析构函数
* Expression实现参数为中缀表达式的构造函数接口
## **BUG report**

## **LOG**
2023.11.30
* 迁移至Ubuntu开发.
2023.12.1
* only need to support int32_t
* 将中缀表达式转成后缀过程中，数字会被[]包裹，变量名会被()包裹，以便于后续解析。

