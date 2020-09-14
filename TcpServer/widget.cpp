#include "widget.h"
#include "ui_widget.h"
#include<QFileDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    tcpSer =nullptr;
    tcpSock=nullptr;
    tcpSer = new QTcpServer(this);
    tcpSer->listen(QHostAddress::Any,8888);
    setWindowTitle("server");
    ui->pushButtonSend->setEnabled(false);
    ui->pushButtonClose->setEnabled(false);
    ui->pushButtonSendFile->setEnabled(false);
    connect(tcpSer,&QTcpServer::newConnection,
                [=]()
                {
                    tcpSock = tcpSer->nextPendingConnection();

                    QString ip=tcpSock->peerAddress().toString();
                    qint16 port = tcpSock->peerPort();
                    QString temp = QString("[ip:%1,port:%2]:连接成功").arg(ip).arg(port);
                    ui->pushButtonSend->setEnabled(true);
                    ui->pushButtonClose->setEnabled(true);                

                    ui->textRead->setText(temp);
                    connect(tcpSock,&QTcpSocket::readyRead,
                            [=](){
                                    QByteArray arr=tcpSock->readAll();
                                    ui->textRead->append(arr);

                                 }
                            );
                }

            );
    udpSock = new QUdpSocket(this);
    udpSock->bind(8888);
    connect(udpSock,&QUdpSocket::readyRead,this,&Widget::onReadgram);
    connect(&timer,&QTimer::timeout,[=](){
        timer.stop();
        sendData();
    });


}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButtonSend_clicked()
{
    if(tcpSer!=nullptr){
        tcpSock->write(ui->textInput->toPlainText().toUtf8().data());
    }
}

void Widget::on_pushButtonClose_clicked()
{   if(tcpSock!=nullptr){
        tcpSock->disconnect();
        tcpSock->close();
    }
}

void Widget::onReadgram()
{
    char buf[1024]={0};
    quint16 port;
    QHostAddress address;
    quint64 len= udpSock->readDatagram(buf,sizeof(buf),&address,&port);
    if(len>0){
        QString str = QString("[ip:%1,port:%2]:%3").arg(address.toString()).arg(port).arg(buf);
        ui->textRead->append(str);
    }

}

void Widget::on_pushButton_clicked()
{
    QString ip=ui->lineEditUdpID->text();
    qint16 port = ui->lineEditUdpPort->text().toInt();

    QString str=ui->textInput->toPlainText();
    udpSock->writeDatagram(str.toUtf8(),QHostAddress(ip),port);

}

void Widget::on_pushButtonChooseFi_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"open","../");
    if(!filePath.isEmpty()){
        fileName.clear();
        fileSize=0;
        QFileInfo fileInfo(filePath);
        fileName=fileInfo.fileName();
        fileSize = fileInfo.size();
        qDebug()<<fileSize;
        sendSize = 0;

        file.setFileName(filePath);
        if(file.open(QIODevice::ReadOnly)){
            ui->textRead->append("此时发送的文件路径是："+filePath);
            ui->lineEditPath->setText(filePath);
        }
    }
}

void Widget::on_pushButtonSendFile_clicked()
{
    QString headMassage = QString("%1##%2").arg(fileName).arg(fileSize);
    qint64 len = tcpSock->write(headMassage.toUtf8().data());
    if(len>0){
        timer.start(20);
    }
}

void Widget::sendData()
{
    qint64 len=0;
    ui->textRead->append("文件正在发送");
    do{
        char buff[4*1024]={0};
        len=0;
        len = file.read(buff,sizeof(buff));
        len = tcpSock->write(buff,len);
        sendSize+=len;
    }while(len>0);
    if(sendSize==fileSize){
        ui->textRead->append("已发送完毕");
        ui->pushButtonSendFile->setEnabled(false);
        file.close();
    }
}

void Widget::on_lineEditPath_textChanged(const QString &arg1)
{
    ui->pushButtonSendFile->setEnabled(true);
}
