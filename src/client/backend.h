#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QSslSocket>

class BackEnd : public QObject {
    Q_OBJECT
public:
    virtual ~BackEnd() {}
    explicit BackEnd(QObject *parent = nullptr);
signals:

public slots:
    bool authenticate(QString email, QString password);
private:
    qint64 write(const QByteArray &data);
    QByteArray read(qint64 maxSize);
    void connect();
    QSslSocket socket;
    bool connected;
};

#endif // BACKEND_H
