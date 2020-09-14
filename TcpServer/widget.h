#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include<QFile>
#include<QTimer>
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
    void on_pushButtonSend_clicked();

    void on_pushButtonClose_clicked();

    void onReadgram();

    void on_pushButton_clicked();

    void on_pushButtonChooseFi_clicked();

    void on_pushButtonSendFile_clicked();

    void on_lineEditPath_textChanged(const QString &arg1);

private:
    void sendData();
    Ui::Widget *ui;
    QTcpServer* tcpSer;//监听套接字
    QTcpSocket* tcpSock;//通信套接字
    QUdpSocket* udpSock;

    QFile file;
    QString fileName;
    qint64 fileSize;
    qint64 sendSize=0;
    QTimer timer;
};
#endif // WIDGET_H
