#include <QCoreApplication>
#include <QtHttpServer>
#include <QTcpServer>
#include <QString>

const int latestVersion = 3;

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QHttpServer httpServer{};
    QTcpServer tcpServer{};

    if (!tcpServer.listen(QHostAddress::Any, 8080)) {
        qWarning() << "Tcp server failed to listen\n";
        return 1;
    }

    if (!httpServer.bind(&tcpServer)) {
        qWarning() << "Http Server Bind Faild\n";
        return 1;
    }

    httpServer.route("/hello", QHttpServerRequest::Method::Get, []() {
        qInfo() << "Get request recieved at /hello";
        return "Hello from HTTP Server::Magdii :)";
    });

    httpServer.route("/version", QHttpServerRequest::Method::Get, []() {
        qInfo() << "Get request recieved at /version";
        QString str = QString::number(latestVersion);
        return str;
    });

    httpServer.route("/download", QHttpServerRequest::Method::Get, []() {
        qInfo() << "Get request recieved at /download";
        return QHttpServerResponse::fromFile("example");
    });

    qInfo() << "Web Server listening on http://localhost:8080";
    return a.exec();
}
