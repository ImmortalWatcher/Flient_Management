# Flient_Management

## 项目结构

```
Flient_Management/
├─ main.cpp                    # 主程序入口，初始化应用程序和登录窗口
├─ CMakeLists.txt              # CMake 构建配置文件
│
├─ 核心模块
│  ├─ DBOperator.h/cpp         # 数据库操作管理类，封装所有数据库操作
│  ├─ WindowFactory.h/cpp      # 窗口工厂类 (单例模式)，负责创建和管理窗口
│  └─ FlightItemWidget.h/cpp   # 航班条目组件，用于显示单个航班信息
│
├─ 登录注册模块
│  ├─ LoginDlg.h/cpp           # 登录对话框，支持用户和管理员登录
│  ├─ LoginDlg.ui              # 登录对话框界面文件
│  ├─ RegisterDlg.h/cpp        # 注册对话框，新用户注册功能
│  └─ RegisterDlg.ui           # 注册对话框界面文件
│
├─ 用户主窗口模块
│  ├─ UserMainWindow.h/cpp     # 用户主窗口，包含航班查询、订单管理、收藏等功能
│  └─ UserMainWindow.ui        # 用户主窗口界面文件
│     ├─ 航班查询页面            # 搜索和浏览航班，支持预订和收藏
│     ├─ 我的订单页面            # 查看订单，支持取消支付和改签
│     ├─ 我的收藏页面            # 管理收藏的航班，支持去支付和取消收藏
│     └─ 个人中心页面            # 查看和编辑个人信息，账户充值
│
├─ 管理员主窗口模块
│  ├─ AdminMainWindow.h/cpp    # 管理员主窗口，包含航班管理、订单查看、用户管理等功能
│  └─ AdminMainWindow.ui       # 管理员主窗口界面文件
│     ├─ 航班管理页面            # 查看、编辑、删除航班信息
│     ├─ 订单查看页面            # 查看所有订单，支持按状态筛选
│     ├─ 用户管理页面            # 查看和管理用户信息
│     └─ 数据统计页面            # 统计航班、订单、用户等数据
│
├─ 资源文件
│  ├─ figures.qrc              # 界面图标资源文件配置
│  ├─ avatars.qrc              # 用户头像资源文件配置
│  └─ source/                  # 资源文件目录
│     ├─ databases/            # 数据库相关文件
│     │  └─ dbSchema.sql       # 数据库表结构定义和测试数据
│     ├─ figures/              # 界面图标资源
│     └─ avatars/              # 用户头像资源 (15 个可选头像)
```
