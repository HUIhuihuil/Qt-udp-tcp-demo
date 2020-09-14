#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QFile>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonClose_clicked();
    void onReadgram();

    void on_pushButton_clicked();

    void on_pushButtonRecive_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket* tcpSocket;
    QUdpSocket* udpSocket;

    QFile file;
    QString fileName;
    qint64 fileSize;
    qint64 sendSize;
    qint64 reciveSize;
    bool istart;
    bool isrecive;

};
#endif // WIDGET_H
