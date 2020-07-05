#ifndef EMAIL_H
#define EMAIL_H

#include <QObject>
#include "mimetypemanager.h"

#define DEF_PROP(_x) \
QString _x() const { return _ ## _x; } \
void set ## _x(const QString& s) { _ ## _x = s; } 

class Email : public QObject
{
    Q_OBJECT

    QString _mailto = "spammer@spam.com";
    QString _subject = "Photo";
    QString _bcc;
    QString _body;

    Q_PROPERTY(QString mailto READ mailto WRITE setmailto);
    Q_PROPERTY(QString subject READ subject WRITE setsubject);
    Q_PROPERTY(QString bcc READ bcc WRITE setbcc);
    Q_PROPERTY(QString body READ body WRITE setbody);

    DEF_PROP(mailto);
    DEF_PROP(subject);
    DEF_PROP(bcc);
    DEF_PROP(body);
    
public:
    explicit Email(QObject* parent = 0, const QString& mimeTypesPath = ":/email/mime.types");
    
    Q_INVOKABLE void open(const QVariant& imgvar);

signals:

    void composerOpened(bool successful);

private:

    MimeTypeManager* mimeTypes;
};

#endif // EMAIL_H
