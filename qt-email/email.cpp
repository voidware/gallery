#include "email.h"
#include <stdio.h>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QtQml>
#include <QTemporaryFile>
#include <QVariant>
#include <QImageWriter>
#include <QImage>
#include <QClipboard>
#include <QGuiApplication>
#include <QDebug>

#include <assert.h>
#include "mimetypemanager.h"

#define URI "Email"

void registerEmail()
{
    qmlRegisterType<Email>(URI, 1, 0, "Email");
}

Email::Email(QObject*parent, const QString& mimeTypesPath) :
    QObject(parent),
    mimeTypes(new MimeTypeManager(mimeTypesPath))
{
}

#define BOUNDARY1 "MOOSE1"
#define BOUNDARY2 "MOOSE2"
#define CRLF "\r\n"

void Email::open(const QVariant& imgvar)
{
    QImage img;
    QList<QString> attachments;    
    QTemporaryFile f("XXXXXX.jpg");
        
    if (imgvar.isValid()) img = imgvar.value<QImage>();
    
    if (!img.isNull())
    {
        QString fname;
        if (f.open())
        {
            fname = f.fileName();
            QImageWriter iw(fname);
            iw.setQuality(85);  // jpeg
            bool v = iw.write(img);
            if (v)
            {
                attachments.append(fname);
                qDebug () << "wrote image to " << fname;
            }
            else
            {
                qDebug() << "error writing to temp file " << fname;
            }
        }
        else
        {
            qDebug() << "unable to open temp file";
        }
    }

    QString email;

    // Add sender information

    // Add receiver information
    email.append("To: ");
    email.append(_mailto);
    email.append(CRLF);

    // Add subject
    email.append("Subject: ");
    email.append(_subject);
    email.append(CRLF);

    if (!_bcc.isEmpty())
    {
        email.append("Bcc: ");
        email.append(_bcc);
        email.append(CRLF);
    }

    // Add mime version
    email.append("Mime-Version: 1.0" CRLF);
    
    email.append("Content-Type: multipart/mixed; boundary=\"" BOUNDARY1 "\"" CRLF CRLF);
    email.append("--" BOUNDARY1 CRLF);

    /*
    email.append("Content-Type: multipart/alternative; boundary=\"" BOUNDARY2 "\"" CRLF CRLF);
    email.append("--" BOUNDARY2 CRLF);
    */

    email.append("Content-Transfer-Encoding: quoted-printable\r\n");
    email.append("Content-Type: text/plain;\r\n");
    email.append("        charset=iso-8859-1\r\n\r\n");

    email.append(_body);
    //email.append("\n");
    
    email.append(CRLF CRLF);

    //email.append("--" BOUNDARY1 CRLF);

    // Add attachments
    foreach (QString filePath, attachments) 
    {
        QFileInfo fileInfo(filePath);
        QString fileName   = fileInfo.fileName();
        QString fileSuffix = fileInfo.suffix();
        QString mimeType   = mimeTypes->mimeTypeFromExtension(fileSuffix);

        QFile attachmentFile(filePath);
        if (attachmentFile.open(QIODevice::ReadOnly) == false) 
        {
            qCritical() << "Failed loading attachment " << filePath;
            emit composerOpened(false);
            return;
        }

        QByteArray fileData = attachmentFile.readAll();
        attachmentFile.close();

        /*
        email.append("\r\n--" BOUNDARY "\r\n");
        email.append("Content-Type: multipart/mixed;\r\n");
        email.append("        boundary=\"" BOUNDARY "\"\r\n\r\n");
        */

        email.append("--" BOUNDARY1 "\r\n");
        email.append("Content-Disposition: inline;\r\n");
        email.append("        filename=\"").append(fileName).append("\"\r\n");
        email.append("Content-Type: ").append(mimeType).append(";\r\n");
        email.append("        name=\"").append(fileName).append("\"\r\n");
        email.append("Content-Transfer-Encoding: base64\r\n\r\n");

        email.append(fileData.toBase64());
    }


    //printf("11## email url '%s'\n", email.toStdString().c_str());

    // Create temporary file and open it in the user's default email composer
    QString tmpFilePath = QDir::tempPath().append(QString("/ComposedEmail-%1.eml").arg(QDateTime::currentDateTime().toTime_t()));
    QFile tmpFile(tmpFilePath);
    if (tmpFile.open(QIODevice::WriteOnly) == false) 
    {
        qCritical() << "Failed opening temp file for email composing:" << tmpFilePath;

        emit composerOpened(false);
        return;
    }

    tmpFile.write(email.toLatin1());
    tmpFile.close();

    //printf("#### email url '%s'\n", tmpFilePath.toStdString().c_str());

    emit composerOpened(QDesktopServices::openUrl(QUrl(tmpFilePath)));
}

QString Email::save(const QVariant& imgvar)
{
    QString fname;
    QImage img;
    QTemporaryFile f("XXXXXX.jpg");
        
    if (imgvar.isValid()) img = imgvar.value<QImage>();
    
    if (!img.isNull())
    {
        if (f.open())
        {
            fname = f.fileName();
            QImageWriter iw(fname);
            iw.setQuality(85);  // jpeg
            bool v = iw.write(img);
            if (v)
            {
                f.setAutoRemove(false); // leave file there
                qDebug () << "wrote image to " << fname;
                
                QClipboard *clipboard = QGuiApplication::clipboard();
                assert(clipboard);

                // make markdown reference to image
                QString s = "![](file://";
                s += fname;
                s += ")";
                clipboard->setText(s);
            }
            else
            {
                qDebug() << "error writing to temp file " << fname;
                fname.clear();
            }
        }
        else
        {
            qDebug() << "unable to open temp file";
        }
    }
    return fname;
}
