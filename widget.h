/**
 * @file widget.h
 * @author ws01
 * @email 01@ws01.ltd
 * @date 2021/5/10
 */

#ifndef CMAKETOQMAKE_WIDGET_H
#define CMAKETOQMAKE_WIDGET_H

#include <QWidget>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    ~Widget() override;

private slots:
    void selectQtDir();
    void selectSourceDir();
    void toCmake();
    void toQmake();
    void updateData();

private:
    void findFile(const QString &dirPath);

private:
    Ui::Widget *ui;

    QString rootDirPath_;
    QString proPath_;
    QStringList sourceList_;
    QStringList headerList_;
    QStringList resourceList_;
    QStringList uiList_;
};


#endif //CMAKETOQMAKE_WIDGET_H
