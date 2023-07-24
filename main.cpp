#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QPdfDocument>
#include <QPdfLinkModel>
#include <QUrl>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("qpdftool");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("PDF utility with random features");
    parser.addHelpOption();
    parser.addVersionOption();

    // An option with a value
    QCommandLineOption withWhat(QStringList() << "w" << "find-files-with",
                                QCoreApplication::translate("main", "Find PDF files in CWD with <feature>, which can be 'links' 'non-numeric-page-labels' or 'untrimmed-page-labels'"),
                                QCoreApplication::translate("main", "feature"));
    parser.addOption(withWhat);

    // Process the actual command line arguments given by the user
    parser.process(app);

    if (!parser.isSet(withWhat)) {
        parser.showHelp();
        return -1;
    }

    enum LookFor {
        LFNone,
        LFLinks,
        LFNonNumericPageLabels,
        LFUntrimmedPageLabels
    };

    LookFor lf = LFNone;
    {
        QString withWhatFeature = parser.value(withWhat);
        if (withWhatFeature == "links")
            lf = LFLinks;
        else if (withWhatFeature == "non-numeric-page-labels")
            lf = LFNonNumericPageLabels;
        else if (withWhatFeature == "untrimmed-page-labels")
            lf = LFUntrimmedPageLabels;
        else {
            qFatal() << "unknown feature" << withWhatFeature;
            return -2;
        }
        qDebug() << "starting in CWD" << QDir::currentPath() << "looking for" << withWhatFeature;
    }

    QDirIterator it(QDir::currentPath(), {"*.pdf"}, QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        auto fi = it.nextFileInfo();
        QPdfDocument doc;
        doc.load(fi.absoluteFilePath());
        if (doc.pageCount() > 1) {
            switch (lf) {
            case LFLinks: {
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
                break;
            }
            case LFNonNumericPageLabels: {
                QStringList nnpl;
                const int pc = doc.pageCount();
                for (int i = 0; i < pc; ++i) {
                    QString l = doc.pageLabel(i);
                    bool ok = false;
                    l.toInt(&ok);
                    if (!ok)
                        nnpl << l;
                }
                if (!nnpl.isEmpty())
                    qDebug() << fi.filePath() << "has" << doc.pageCount() << "pages: the non-numeric ones are" << nnpl;
                break;
            }
            case LFUntrimmedPageLabels: {
                QStringList upl;
                const int pc = doc.pageCount();
                for (int i = 0; i < pc; ++i) {
                    QString l = doc.pageLabel(i);
                    if (l.trimmed() != l)
                        upl << l;
                }
                if (!upl.isEmpty())
                    qDebug() << fi.filePath() << "has" << doc.pageCount() << "pages: the untrimmed labels are" << upl;
                break;
            }
            case LFNone:
                break;
            }


        }
    }

//    return a.exec();
    return 0;
}
