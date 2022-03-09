#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QPdfDocument>
#include <QtPdf/private/qpdflinkmodel_p.h>
#include <QUrl>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "starting in CWD" << QDir::currentPath();

    QDirIterator it(QDir::currentPath(), {"*.pdf"}, QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        auto fi = it.nextFileInfo();
        QPdfDocument doc;
        doc.load(fi.absoluteFilePath());
        if (doc.pageCount() > 1) {
            QPdfLinkModel linkModel;
            linkModel.setDocument(&doc);
            const int linkCount = linkModel.rowCount({});
            if (!linkCount)
                continue;
            int internalLinkCount = 0;
            int externalLinkCount = 0;
            for (int i = 0; i < linkCount; ++i) {
                QModelIndex idx = linkModel.index(i, 0);
//                qDebug() << "        " << linkModel.data(idx, int(QPdfLinkModel::Role::Url)) << linkModel.data(idx, int(QPdfLinkModel::Role::Page));
                if (linkModel.data(idx, int(QPdfLinkModel::Role::Url)).toUrl().isValid())
                    ++externalLinkCount;
                else if (linkModel.data(idx, int(QPdfLinkModel::Role::Page)).toInt() >= 0)
                    ++internalLinkCount;
                else
                    qWarning() << "weird link: neither URL nor page number";
            }
            qDebug() << fi.filePath() << "has" << doc.pageCount() << "pages," << linkCount << "links :"
                     << internalLinkCount << "internal, " << externalLinkCount << "external";
        }
    }

//    return a.exec();
    return 0;
}
