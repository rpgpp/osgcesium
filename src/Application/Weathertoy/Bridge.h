#pragma once

#include <QObject>

class Bridge : public QObject
{
    Q_OBJECT
public:
    static Bridge* instance();
    ~Bridge();
public slots:
    void showMsgBox();
    void eval(QString method,QString json);
private:
    Bridge();
};
