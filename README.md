# README

2023年图灵班轮转项目——程序合成相关。

这个项目主要涉及程序合成领域中最为常用的一类技术，基于枚举的程序合成。该项目的形式与 Lab 类似：我们提供了一个基础的程序合成框架，而参与者的任务是在这个框架的基础上实现一些经典的程序合成算法，在一个简单的数据集上分析这些算法的差异，并对一个相对前沿的问题展开探索。

该项目主要分为四个部分，其中前三部分是必须完成的内容，第四部分是可选的内容。

1. 安装这一项目，简单了解 SyGuS 程序合成框架。
2. （经典技术）阅读我们提供的自底向上枚举的程序合成器，并将两个经典优化技术，反例制导的程序合成与观察等价法，集成到该实现中。
3. （前沿技术）学习 FTA 相关的内容，并补完我们提供的部分实现。


本次轮转的时长为五周，因此我们推荐以下两种时间安排：

1. 如果不打算探索第四部分，则可以给前三部分分配一周、一周半和两周半的时间。
2. 如果计划探索第四部分，则可以给前三部分分配半周、一周和一周的时间。

## Step1：安装+SyGuS

**安装**：该项目支持 Linux (推荐) 和 MacOS。如果你手上没有对应系统的电脑或者懒得配环境，可以联系我要一个远程服务器的账号。

该项目需要版本不低于 9.1 的 gcc，版本不低于 3.13 的 CMake，以及 python3。初次以外，它还依赖于 jsoncpp, glog, 以及 Python 库 pyparsing。 在 Linux 中，你可以通过如下命令来安装它们。

```bash
$ apt-get install libjsoncpp-dev libgoogle-glog-dev
$ pip3 install pyparsing
```

推荐大家在 IDE CLion 中打开这一项目。打开以后，需要修改以下配置：

1. 如果系统是 MacOS，则需要修改 `CMakeLists.txt` 中与 jsoncpp 与 glog 相关的部分（20,21,24,25行）：将这些地址修改为这些库在本地的安装地址。
2. 将 `basic/config.cpp` 中的 `KSourcePath` 修改为本地的项目地址。

该项目中的源代码按照如下方式组织。你在完成这一项目的过程中不需要知道 `basic` 和 `sygus` 中的实现细节，因此可以先专注于 `tasks` 中的内容。

|    Directory     |               Description                |
| :--------------: | :--------------------------------------: |
| `include/istool` |                所有涉及的头文件。                 |
|     `basic`      |       程序合成中会用到的基础数据结构，包括值、程序、规约等。        |
|     `sygus`      | 涉及 SyGuS 框架的数据结构以及接口，包括一些操作符的定义以及 SyGuS 格式的程序合成任务的 Parser。 |
|     `tasks`      | 该项目的主要部分，包含了两个程序 `task_enumeration.cpp` 和 `task_fta.cpp`。在该项目中，你需要补全或者优化它们 |
|     `tests`      |    一个简单的数据集，里面包含了几个 SyGuS 格式的程序合成任务。     |

若要测试是否安装成功，大家可以编译运行 `tasks/task_enumeration.cpp`。如果安装成功，则它会默认求解任务 `tasks/plus.sl`并在最后输出一个对两个输入求和的程序以及合成需要的时间开销，如下所示。

```
Result: +(Param0,Param1)
Time Cost: 0.000827 seconds
```

此外，大家可以通过修改 `task/task_enumeration.cpp` 中的任务路径（59行）来尝试求解其他的任务。

**SyGuS**: `tests` 文件中的所有任务都服从语法制导的程序合成（SyGuS）框架。请阅读 [软件分析第16讲课件](https://xiongyingfei.github.io/SA/2022/16_program_synthesis_enumerative.pdf) 中的前 20 页来了解这一框架，并在阅读后回答以下问题：

**Q1.1**: 任务 `max3.sl` 的目标程序是什么？

**Q1.2**: 任务 `PRE_3_10.sl` ，`PRE_3_100.sl`， 和 `PRE_3_1000.sl` 都是有关 64 位布尔向量的程序合成任务。它们有什么样的不同？

## Step 2: 经典技术 

**自底向上的枚举**：请阅读 [软件分析第16讲课件](https://xiongyingfei.github.io/SA/2022/16_program_synthesis_enumerative.pdf) 中的 27-29 页来了解自底向上的程序合成技术。文件 `tasks/task_enumeration.cpp` 提供了该合成算法的一种实现，请阅读理解该实现的主要部分。

**CEGIS**: 反例制导的程序合成（CEGIS） 是一种经典的程序合成优化技术。请阅读 [软件分析第16讲课件](https://xiongyingfei.github.io/SA/2022/16_program_synthesis_enumerative.pdf) 中的 21 页来了解该框架，并在 `tasks/task_enumeration.cpp` 中将其实现。

**观察等价法**：观察等价法是自底向上枚举过程中的一种剪枝技术。请阅读 [软件分析第16讲课件](https://xiongyingfei.github.io/SA/2022/16_program_synthesis_enumerative.pdf) 中的 32 页来了解该框架，并在 `tasks/task_enumeration.cpp` 中将其实现。

在实现结束后，请回答以下问题：

**Q2:** 为什么 CEGIS 和观察等价法能够提升枚举的效率？它们在任何情况下都能提升效率吗？在我们提供的数据集上，它们带来的效率提升是否显著？如果不显著，为什么？

## Step 3: 前沿技术

**FTA**: 有限树自动机（FTA）是一种相对前沿的枚举方法。请阅读 [软件分析第17讲课件](https://xiongyingfei.github.io/SA/2022/17_program_synthesis_representation.pdf) 中的前 27 页来了解空间表示法和有限树自动机，并补全我们在 `tasks/task_fta.cpp` 中提供的部分实现。

在实现结束后，请回答以下问题：

**Q3.1**: FTA 和观察等价法都是利用程序在目前样例上的输出来剪枝。它们等价吗？如果不等价，FTA 的优势/劣势在哪里？

**Q3.2**: FTA 时间开销的瓶颈在哪里？