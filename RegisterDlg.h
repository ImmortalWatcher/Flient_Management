#ifndef REGISTERDLG_H
#define REGISTERDLG_H

#include <QDialog>

namespace Ui { class RegDlg; }

class RegDlg : public QDialog {
    Q_OBJECT

public:
    explicit RegDlg(QWidget *parent = nullptr);
    ~RegDlg();

private:
    Ui::RegDlg *ui;
};

#endif // REGISTERDLG_H
