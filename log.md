## **TODO list**
* (Ok)暂时不考虑表达式中有负数的情况
* (Ok)禁止tab输入
* (Ok)测试Expression构造函数的内存泄漏情况
* (Ok)实现析构函数
* Expression实现参数为中缀表达式的构造函数接口
* (Ok)取消变量名中文支持
* 禁止使用关键字作为变量名
* 取消Statement构造函数中clearRunTime
## **BUG report**

## **Question list**
* 错误语句也要记录runtime吗
## **LOG**
2023.11.30
* 迁移至Ubuntu开发.
2023.12.1
* only need to support int32_t
* 将中缀表达式转成后缀过程中，数字会被[]包裹，变量名会被()包裹，以便于后续解析。

