# liscpp

一款基于c++实现的Lisp([Clojure](https://en.wikipedia.org/wiki/Clojure))语言解释器。

### 使用方式

支持REPL(Read-Eval-Print-Loop)交互模式和文件执行模式。

REPL交互模式:
* 输入指令
```shell
liscpp
```
* 进入REPL交互界面
```text
user>>
```

文件执行模式:
* 输入指令
```shell
liscpp "hello_world.mal"
```
* 输出结果
```text
Hello world!
```

### 支持功能 && 未来计划

已完成部分：

✅ 内置类型系统：

MalType --> MalPair, MalList, MalVector, MalMap,MalBool, MalInt, MalString, MalSymbol, ...

✅ 三大组件: 解析器(Reader), 求值器(Evaluator), 打印器(Printer)

✅ 流程控制关键字(do, if), 定义关键字(let*, fn*, def!), ...

✅ 大部分标准内建函数: +, -, *, /, prn, println, eval, load-file, ...

待实现：

📝 相关读取器宏

📝 异常捕捉系统

📝 元数据

📝 解释器内存管理优化

📝 解释器LLVM移植
