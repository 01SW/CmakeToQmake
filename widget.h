/**
 * @file widget.h
 * @author ws01
 * @email 01@ws01.ltd
 * @date 2021/5/10
 */

#ifndef CMAKETOQMAKE_WIDGET_H
#define CMAKETOQMAKE_WIDGET_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    ~Widget() override;

private:
    Ui::Widget *ui;
};


#endif //CMAKETOQMAKE_WIDGET_H
