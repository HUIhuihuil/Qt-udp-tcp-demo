#include "widget.h"
#include "ui_widget.h"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    setWindowTitle("client");
    ui->pushButtonSend->setEnabled(false);
    ui->pushButtonClose->setEnabled(false);
    connect(tcpSocket,&QTcpSocket::connected,
            [=](){
        ui->textEditRead->setText("连接成功");
        ui->pushButtonSend->setEnabled(true);
        ui->pushButtonClose->setEnabled(true);

    });
    istart=true;
    isrecive=false;
    connect(tcpSocket,&QTcpSocket::readyRead,[=](){
        QByteArray buf = tcpSocket->readAll();
        ui->textEditRead->append(buf);
        if(istart&&isrecive){
            fileName = QString(buf).section("##",0,0);
            fileSize = QString(buf).section("##",1,1).toUInt();
            reciveSize = 0;
            file.setFileName(fileName);
            if(file.open(QIODevice::WriteOnly)){
                qDebug()<<"OK";}
            istart=false;

        }else if(isrecive){
            quint64 len=file.write(buf);
            reciveSize+=len;
            qDebug()<<reciveSize;
            if(reciveSize==fileSize){
                file.close();
                ui->textEditRead->append("传输文件完成");
                istart=true;
                isrecive=false;
                reciveSize=0;
            }
        }


    });
    connect(tcpSocket,&QTcpSocket::disconnected,[=](){
        ui->textEditRead->append("与服务器连接已断开");
    });
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(8889);
    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::onReadgram);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButtonConnect_clicked()
{
    QString ip=ui->lineEditIp->text();
    qint16 port = ui->lineEditPort->text().toInt();

    tcpSocket->connectToHost(ip,port);

}

void Widget::on_pushButtonSend_clicked()
{
    tcpSocket->write(ui->textEditInput->toPlainText().toUtf8().data());
}

void Widget::on_pushButtonClose_clicked()
{
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
}
void Widget::onReadgram()
{
    char buf[1024]={0};
    quint16 port;
    QHostAddress address;
    quint64 len= udpSocket->readDatagram(buf,sizeof(buf),&address,&port);
    if(len>0){
        QString str = QString("[ip:%1,port:%2]:%3").arg(address.toString()).arg(port).arg(buf);
        ui->textEditRead->append(str);
    }

}

void Widget::on_pushButton_clicked()
{
    QString ip=ui->lineEditIp->text();
    qint16 port = ui->lineEditPort->text().toInt();

    QString str=ui->textEditInput->toPlainText();
    udpSocket->writeDatagram(str.toUtf8(),QHostAddress(ip),port);
}

void Widget::on_pushButtonRecive_clicked()
{
    tcpSocket->write("可以开始传送文件");
    isrecive=true;
}
