# Flient_Management

## 项目结构

```
Flient_Management/
├─ main.cpp                    # 主程序入口
├─ DBOperator.h/cpp            # 数据库操作管理类
├─ 登录模块
│  ├─ LoginDlg.h/cpp           # 登录对话框逻辑代码
│  └─ LoginDlg.ui              # 登录对话框界面文件 (Qt Designer)
├─ 主窗口模块
│  ├─ mainwindow.h/cpp         # 主窗口逻辑代码
│  └─ mainwindow.ui            # 主窗口界面文件 (Qt Designer)
└─ 注册模块
   ├─ RegisterDlg.h/cpp        # 注册对话框逻辑代码
   └─ RegisterDlg.ui           # 注册对话框界面文件 (Qt Designer)
```

## 现存问题

- 应用窗口界面无法跟随放大 / 缩小 (自适应), 原因在于各控件是 "自由放置" 的, 后续需要设置严格的 "布局"

## 注意事项

- 图片均采用 (.png) 格式存储, 兼容性高