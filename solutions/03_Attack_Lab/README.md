# attack lab

## attack 方式

1. 如何操作

```bash
# 假设 foo.s 是我们想要注入的代码
vim foo.s

# 利用 gcc 生成对应的字节码 foo.o
gcc -c foo.s

# 通过 objdump 来查看其内容，可以看到对应的字节码
objdump -d foo.o | less

# 然后需要把十六进制代码转换成字符串这样我们可以写在程序里
./hex2raw -i inputfile -o outputfile
```

2. ctarget code-injection(代码注入) attacks

原理: C语言中执行函数时会分配栈帧空间并更改栈顶位置,所有函数申请的局部变量,都会保存在分配到的栈帧中.如果在一个函数中申请一个数组,输入的数据大于数组的长度,多出来的数据会覆盖其余的栈帧内容.缓存溢出漏洞即利用该情况,程序接收了大于数组长度的数据,多出来的数据覆盖了栈帧上的其他内容,比如返回地址

3. rtarget return-oriented-programming(ROP,返回导向编程) attacks

4. 文件作用

ctarget: 用来做代码注入攻击的程序
rtarget: 用来做 ROP 攻击的程序
cookie.txt: 一个 8 位的 16 进制代码，用来作为攻击的标识符
farm.c: 用来找寻 gadget 的源文件
hex2raw: 用来生成攻击字符串的程序

## 基础知识

用来传参数的寄存器：%rdi, %rsi, %rdx, %rcx, %r8, %r9
保存返回值的寄存器：%rax
被调用者保存状态：%rbx, %r12, %r13, %r14, %rbp, %rsp
调用者保存状态：%rdi, %rsi, %rdx, %rcx, %r8, %r9, %rax, %r10, %r11
栈指针：%rsp
指令指针：%rip

