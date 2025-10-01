#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTcpServer>
#include <QString>
#include <QFile>

const int latestVersion = 2;

QString downloadUpdate(){
    QNetworkAccessManager manager{};
    QNetworkRequest download{QUrl{"http://localhost:8080/download"}};
    QNetworkReply* downloadReply = manager.get(download);
    QString str;

    QObject::connect(downloadReply, &QNetworkReply::finished, [downloadReply, &str](){
        QTextStream textStream{downloadReply};
        while(!textStream.atEnd())
            textStream >> str;
    });
    return str;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QNetworkAccessManager manager{};
    QNetworkRequest request{QUrl{"http://localhost:8080/version"}};

    QNetworkReply* reply = manager.get(request);

    QNetworkRequest download{QUrl{"http://localhost:8080/download"}};
    QNetworkReply* downloadReply = manager.get(download);

    QObject::connect(reply, &QNetworkReply::finished, [reply, downloadReply](){
        QString str;
        QTextStream textStream{reply};
        textStream >> str;
        qInfo() << "Server Version " << str;

        if(str.toInt() == latestVersion){
            qInfo() << "No Updates Available " << str;
        }else if(str.toInt() > latestVersion){
            qInfo() << "System Update Required";


            QObject::connect(downloadReply, &QNetworkReply::finished, [downloadReply, &textStream](){
                quint8 data;
                QDataStream dataStream{downloadReply};
                QFile out("update");
                out.open(QIODevice::WriteOnly);
                QDataStream outStream{&out};
                while(!dataStream.atEnd()){
                    dataStream >> data;
                    outStream << data;
                }
                qInfo() << "Update Completed Check for downloaded file";
            });
        }else{
            qInfo() << "Error Recieving Version";
        }
    });


    return a.exec();
}
