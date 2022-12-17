- $\LaTeX$ of README.md may fail to display on GitHub. For better experience, pls check [report in pdf format.](https://github.com/GuTaoZi/CS205_Project05/blob/master/doc/Report.pdf)

# CS205 C/ C++ Programming Project05

# A Class for Matrices

**Name**: 樊斯特(Fan Site)

**SID**: 12111624

### 项目结构

```
```

## Part 1 - Analysis

### 题目重述&主要思路

本题目要求使用C++语言实现一个具有一定功能的矩阵类。

根据题目描述，题目要求的矩阵乘法需要支持的主要功能为：

1. 支持多通道存储数据
2. 支持多种数据类型
3. 赋值时避免深拷贝(Hard Copy)，安全管理内存
4. 重载基本运算符
5. 不使用深拷贝实现ROI

本项目除完成上述基础要求外，支持以下内容：

6. **支持任意合理重载运算符的数据类型**
7. **跨数据类型运算、转换、赋值**(提供转换函数接口)
8. **异常处理机制**(另提供严格版函数)
9. **子矩阵/掩膜提取两类ROI实现**
10. **逐元素一元/二元自定义运算**(提供运算函数接口)
11. **默认重载软拷贝(Shallow Copy)，提供硬拷贝函数**
12. **更多易用重载运算符**
13. **~~用户友好的足量注释~~**

### 模型假设

项目要求矩阵类需要适用于不同数据类型，因此本项目主体使用类模板完成，笔者考虑该库所可能使用的数据范围作为本项目支持的数据规模，如下：

- 可适用的矩阵尺寸因元素类型而异，因此未设置`rows`和`cols`上限(不抛出`bad alloc exception`即可)。

- 