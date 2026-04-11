# Trajectory Similarity / 轨迹相似度分析系统

**基于弗雷歇距离（Fréchet Distance）的轨迹相似度分析与可视化软件**

*A Trajectory Similarity Analysis and Visualization Software Based on Fréchet Distance*

---

## 项目简介 / Overview

本项目为 **第五届"中国软件杯"大学生软件设计大赛** 参赛作品，旨在解决以经纬度（或经纬度+时间戳）定义的 GPS 轨迹之间的相似性度量问题。系统实现了基于 **离散弗雷歇距离（Discrete Fréchet Distance）** 的核心算法，支持轨迹相似度计算、相似轨迹段/轨迹点查询、轨迹聚类等功能，并提供完整的 GUI 可视化界面。

> This project was developed for the **5th "China Software Cup" National Software Design Competition for College Students**. It addresses the problem of measuring similarity between GPS trajectories defined by coordinates (with or without timestamps). The system implements core algorithms based on the **Discrete Fréchet Distance**, supporting trajectory similarity computation, similar trajectory segment/point queries, trajectory clustering, and provides a full-featured GUI visualization interface.

---

## 核心功能 / Key Features

| 功能 / Feature | 描述 / Description |
|---|---|
| **轨迹相似度计算** | 基于离散弗雷歇距离算法，支持带时间与不带时间两种模式 |
| *Trajectory Similarity* | Discrete Fréchet Distance algorithm with/without temporal dimension |
| **相似轨迹段查询** | 滑窗扫描 + 合并策略，自动定位两条轨迹间的相似子段 |
| *Similar Segment Detection* | Sliding window + merge strategy for locating similar sub-trajectories |
| **最近轨迹点查询** | O(n*m) 全点对距离搜索，返回最相似的点对集合 |
| *Nearest Point Query* | Brute-force all-pairs search returning the most similar point pairs |
| **层次聚类 (Agglomerative)** | 基于改进 Hausdorff 距离的自底向上聚类 |
| *Hierarchical Clustering* | Bottom-up clustering using modified Hausdorff distance |
| **谱聚类 (Spectral)** | 基于 Eigen 库的特征分解 + K-Means 后处理 |
| *Spectral Clustering* | Eigen-decomposition via Eigen library with K-Means post-processing |
| **空间四叉树索引** | 递归四分法对轨迹进行空间类型编码 |
| *Spatial Quad-tree Indexing* | Recursive quadrant-based spatial type encoding |
| **地图可视化** | 集成 WebKit 地图组件，支持轨迹渲染与交互 |
| *Map Visualization* | Integrated WebKit map component with trajectory rendering |
| **数据持久化** | SQLite 数据库存储，支持导入/导出 CSV |
| *Data Persistence* | SQLite storage with CSV import/export support |

---

## 技术栈 / Tech Stack

```
Frontend (GUI)    : Qt 5.x (Qt Widgets, Qt WebKit, Qt SQL, Qt Network)
Backend (Algorithm): C++11 / OpenMP
Linear Algebra     : Eigen 3
Computer Vision    : OpenCV 3.10 (test module)
Database          : SQLite 3 (via QtSql)
Build System       : qmake (Qt Project file .pro)
Target Platform    : Windows / Linux (Desktop Application)
```

---

## 算法原理 / Algorithm Details

### 1. 离散弗雷歇距离 (Discrete Fréchet Distance)

弗雷歇距离直观描述为：一个人用绳子牵着一条狗分别沿两条路径行走，所需的最短绳长即为弗雷歇距离。本项目的离散版本采用动态规划实现：

$$
DFD(P, Q) = \max_{i,j} \min \{ d(p_i, q_j), DFD(P_{i-1}, Q_j), DFD(P_i, Q_{j-1}), DFD(P_{i-1}, Q_{j-1}) \}
$$

对于带时间戳的轨迹，欧氏距离扩展为时空联合距离：

$$
d'(p_i, q_j) = (\Delta lon)^2 + (\Delta lat)^2 + \alpha \cdot \Delta t
$$

其中 $\alpha$ 为时空权重系数，通过两条轨迹的空间跨度与时间跨度的比值自动标定 (`calCoef()`)。

### 2. 相似轨迹段查找 (Similar Segment Search)

采用多尺度滑窗策略：
- 小规模轨迹（总点数 ≤200）：步长 gap=1
- 大规模轨迹：步长 gap=5

通过 `calculateSec()` 计算所有窗口对的局部弗雷歇距离，再由 `mergeChange()` 将相邻且均低于阈值（全局 DFD × 0.1）的窗口合并为更长候选段。

### 3. 改进 Hausdorff 距离聚类 (Modified Hausdorff Distance Clustering)

使用基于相对位置的邻域匹配策略替代标准 Hausdorff 距离，取第 88% 分位数作为最终距离度量，增强鲁棒性。

### 4. 谱聚类 (Spectral Clustering)

构建高斯核亲和矩阵 $K_{ij} = \exp(-d_{ij}^2 / (2\sigma_i \sigma_j))$，归一化后进行特征分解，利用特征值间隙估计最优聚类数。

---

## 项目结构 / Project Structure

```
Trajectory-similarity/
├── cnsoft.pro                  # 顶层项目文件 (subdirs: main + test)
├── main/                       # 主应用程序 (GUI 客户端)
│   ├── main.pro                #   主程序 .pro 文件
│   ├── main.cpp                #   程序入口
│   ├── core.h / core.cpp       #   核心：弗雷歇距离 / 聚类算法
│   ├── sequence.h / sequence.cpp #   数据结构：Point / Sequence
│   ├── csv.h / csv.cpp         #   CSV 文件解析器
│   ├── database.h / database.cpp #   SQLite 数据库操作
│   ├── mainwindow.*            #   主界面窗口
│   ├── canvas.h / canvas.cpp   #   自定义绘图控件
│   ├── calwindow.*             #   相似度计算窗口
│   ├── searchwin.*             #   搜索结果窗口
│   ├── detailwin.*             #   详情展示窗口
│   ├── mapwindow.*             #   地图显示窗口 (WebKit)
│   ├── settingwin.*            #   设置窗口
│   ├── cloud.h / cloud.cpp     #   云端功能模块
│   ├── client.h / client.cpp   #   网络客户端
│   ├── clusterdemo.*           #   聚类演示模块
│   ├── lcswidget.*             #   LCS 可视化控件
│   └── html/                   #   前端资源 (HTML/JS/CSS/图标)
│       ├── main.html / map.html / search.html
│       ├── cal.js / lcm.js / map.js / search.js
│       └── pointico/           #     轨迹点图标资源
├── server/                     # TCP 服务端 (旧版)
│   ├── TcpServerer.pro
│   ├── tcpserver.* / tcpsocket.*
│   └── eventdispatcher_libev/  #   libev 事件分发
├── postgresserver/             # TCP 服务端 (PostgreSQL 版)
│   └── (结构同 server/)
├── test/                       # 测试模块
│   ├── test.pro
│   ├── test.cpp                #   含谱聚类测试 / OpenCV 测试
│   └── 3rdParty/               #   OpenCV 3.10 库文件
└── 测试数据/                    # 示例 CSV 数据集
    ├── y=x(不含时间).csv        #   直线轨迹
    ├── y=x2(不含时间).csv       #   抛物线轨迹
    ├── y=sinx(近似不含时间).csv  #   正弦曲线轨迹
    ├── 双轨迹段比较*.csv         #   两轨迹对比样本
    └── 三轨迹段比较*.csv         #   三轨迹对比样本
```

---

## 编译构建 / Build Instructions

### 前置依赖 / Prerequisites

| 依赖 / Dependency | 最低版本 / Min Version |
|---|---|
| Qt | 5.x (需含 webkitwidgets, sql, network, printsupport 模块) |
| g++ (GCC) | 4.8+ (C++11 支持) |
| OpenMP | libgomp (可选，并行加速) |
| Eigen 3 | 内置于 `main/ThirdParty/Eigen/` |
| OpenCV 3.10 | 仅 test 模块需要 (内置于 `test/3rdParty/`) |

### 构建步骤 / Build Steps

```bash
# 1. 使用 Qt Creator 打开项目
#    Open cnsoft.pro in Qt Creator

# 2. 或命令行构建 / Command line build
qmake cnsoft.pro
make -j$(nproc)

# 3. 仅构建主程序 / Build main app only
cd main && qmake main.pro && make

# 4. 构建测试程序 / Build test module
cd test && qmake test.pro && make
```

> **注意 / Note**: 当前环境未安装 Qt，无法在此服务器上直接编译。请在安装了 Qt 5.x 的开发环境中执行上述步骤。
>
> **Note**: Qt is not installed on this server. Please run the build commands in a development environment with Qt 5.x installed.

---

## 使用说明 / Usage

### 数据格式 / Data Format

CSV 文件格式（支持两种）：

```
# 不含时间戳 / Without timestamp:
longitude,latitude
116.397128,39.916527
116.398234,39.917635
...

# 含时间戳 / With timestamp:
longitude,latitude,time
116.397128,39.916527,20160601 13:00:00
116.398234,39.917635,20160601 13:05:22
...
```

### 操作流程 / Workflow

1. **导入数据** — 通过菜单或拖拽导入 CSV 格式轨迹文件
2. **预览轨迹** — 在地图或画布上查看已导入轨迹
3. **选择比对** — 在表格中勾选两条待比较轨迹
4. **计算相似度** — 点击计算按钮获取弗雷歇距离
5. **查看详情** — 查看相似轨迹段、最近点等详细信息
6. **聚类分析** — 对多条轨迹进行层次/谱聚类
7. **导出结果** — 将计算结果导出或保存至数据库

---

## 已修复的关键问题 / Critical Fixes Applied

在本次代码审查中修复了以下关键缺陷：

| # | 问题 / Issue | 文件 / File | 严重程度 / Severity |
|---|---|---|---|
| 1 | **PI 常量精度不足** (`3.14` → `3.14159265...`) | `core.h:24` | 高 - 影响所有地理距离计算 |
| 2 | **前缀和自距离 bug** (`pts[i-1], pts[i-1]` → `pts[i-1], pts[i]`) | `sequence.cpp:261` | 致命 - 导致所有前缀和为零，聚类完全失效 |
| 3 | **LCS 算法循环边界错误** (`j <= p.pointsNum` → `j <= q.pointsNum`) | `core.cpp:322` | 高 - 不同长度轨迹产生越界/漏算 |
| 4 | **hardCluster 经纬度混淆** (经度与纬度比较) | `core.cpp:710` | 高 - 空间分类结果错误 |
| 5 | **时间转换系数误差** (分钟 `*0.016` → `/60.0`, 秒 `*0.0002` → `/3600.0`) | `core.cpp:1131` | 中 - 时间距离系统性偏差 ~4%~28% |
| 6 | **SQL 类型拼写错误** (`vachar` → `varchar`) | `database.cpp:606` | 中 - 建表语句可能失败 |
| 7 | **字段名拼写错误** (`simliarity` → `similarity`) | `core.h:47` | 低 - 影响可读性与一致性 |

以上修复同步应用于 `main/`, `server/`, `postgresserver/` 三个目录。

---

## 团队信息 / Team

| 项目 / Item | 内容 / Info |
|---|---|
| **赛题 / Competition** | 第五届"中国软件杯"大学生软件设计大赛 |
| **软件名称 / Software Name** | 基于"弗雷歇距离"的轨迹相似度分析软件 |
| **开发团队 / Team** | Kryptonite |
| **开发时间 / Period** | 2016年4月 - 6月 |
| **联系邮箱 / Contact** | vergil5750@whu.edu.cn |

---

## 开源许可 / License

本项目仅供学习交流参考使用。

>This project is intended for educational and research purposes only.
