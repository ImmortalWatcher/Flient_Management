# Flient_Management

## 项目结构

```
Flient_Management/
├─ main.cpp                    # 主程序入口
├─ DBOperator.h/cpp            # 数据库操作管理类
├─ WindowFactory.h/cpp         # 窗口工厂类 (单例模式，负责创建窗口)
├─ figures.qrc                 # 资源文件配置
├─ CMakeLists.txt              # CMake 构建配置文件
├─ 登录模块
│  ├─ LoginDlg.h/cpp           # 登录对话框逻辑代码
│  └─ LoginDlg.ui              # 登录对话框界面文件 (Qt Designer)
├─ 注册模块
│  ├─ RegisterDlg.h/cpp        # 注册对话框逻辑代码
│  └─ RegisterDlg.ui           # 注册对话框界面文件 (Qt Designer)
├─ 用户主窗口模块
│  ├─ UserMainWindow.h/cpp     # 用户主窗口逻辑代码
│  └─ UserMainWindow.ui        # 用户主窗口界面文件 (Qt Designer)
├─ 管理员主窗口模块
│  ├─ AdminMainWindow.h/cpp    # 管理员主窗口逻辑代码
│  └─ AdminMainWindow.ui       # 管理员主窗口界面文件 (Qt Designer)
└─ source/                     # 核心源码目录
   ├─ databases/               # 数据库相关文件
   │  └─ dbSchema.sql          # 数据库表结构定义 (用户表和管理员表)
   └─ figures/                 # 界面资源图片
```
