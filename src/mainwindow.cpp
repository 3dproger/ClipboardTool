#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(
                Qt::WindowType::Window |
                Qt::WindowType::WindowSystemMenuHint |
                Qt::WindowType::WindowMinMaxButtonsHint |
                Qt::WindowType::WindowCloseButtonHint |
                Qt::WindowType::WindowStaysOnTopHint);

    QClipboard* clipboard = QGuiApplication::clipboard();

    if (!clipboard)
    {
        qDebug() << "!clipboard";
        QMessageBox::critical(this, tr("Error"), tr("Unable to access the clipboard"));
        return;
    }

    if (clipboard)
    {
        connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::OnClipboardDataChanged);
    }

    OnClipboardDataChanged();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnClipboardDataChanged()
{
    qDebug() << "clipboard data changed";
    ui->textBrowserClipboardText->setText("");
    ui->textBrowserClipboardInfo->setText("");
    ui->labelClipboardImage->setVisible(false);
    ui->textBrowserClipboardText->setVisible(false);

    AddInfoLineStr(tr("Last Update"), QDateTime::currentDateTime().time().toString());

    static const QClipboard::Mode mode = QClipboard::Mode::Clipboard;

    QClipboard* clipboard = QGuiApplication::clipboard();
    if (!clipboard)
    {
        AddInfoLine(tr("Unable to access the clipboard"));
        return;
    }

    const QMimeData* mimeData = clipboard->mimeData(mode);
    if (!mimeData)
    {
        AddInfoLine(tr("Unable to access the clipboard data"));
        return;
    }

    QString resultMimeType;

    if (mimeData->hasText())
    {
        resultMimeType = "text/plain";

        const QByteArray& data = mimeData->data(resultMimeType);
        const QString& text = mimeData->data(resultMimeType);

        AddInfoLineStr(tr("Size in bytes"), tr("%1 bytes").arg(data.size()));
        AddInfoLineInt(tr("Characters"), text.count());

        int lines = text.count('\n');
        if (!text.isEmpty())
        {
            lines++;
        }

        AddInfoLineInt(tr("Lines"), lines);

        AddInfoLineBool(tr("Upper Only"), text.isUpper());
        AddInfoLineBool(tr("Lower Only"), text.isLower());
        AddInfoLineBool(tr("Is Right to Left"), text.isRightToLeft());

        bool hasUnicode = false;

        for (const QChar& c : text)
        {
            if (c.toLatin1() != c)
            {
                hasUnicode = true; // ToDo: may not work properly
            }
        }

        AddInfoLineBool(tr("Has Unicode"), hasUnicode);

        ui->textBrowserClipboardText->setText(text);
        ui->textBrowserClipboardText->setVisible(true);
    }
    else if (mimeData->hasImage())
    {
        resultMimeType = "image/*";

        const QPixmap& pixmap = clipboard->pixmap(mode);
        const QImage& image = clipboard->image(mode);

        ui->labelClipboardImage->setPixmap(pixmap);
        ui->labelClipboardImage->setVisible(true);

        AddInfoLineInt(tr("Width"), pixmap.width());
        AddInfoLineInt(tr("Height"), pixmap.height());

        AddInfoLineStr(tr("Size in bytes"), tr("%1 bytes").arg(image.sizeInBytes()));
        AddInfoLineBool(tr("Alpha"), pixmap.hasAlpha());
        AddInfoLineBool(tr("Alpha Channel"), pixmap.hasAlphaChannel());
        AddInfoLineInt(tr("Depth"), pixmap.depth());
        AddInfoLineStr(tr("Image Format"), FormatToStr(image.format()));
        AddInfoLineBool(tr("Is Grayscale"), image.isGrayscale());
    }
    else
    {
        ui->textBrowserClipboardText->setVisible(true);
    }

    if (resultMimeType.isEmpty())
    {
        resultMimeType = tr("<unknown>");
    }

    AddInfoLineStr(tr("Mime Type"), resultMimeType);
}

QString MainWindow::FormatToStr(QImage::Format format)
{
    switch (format)
    {
    case QImage::Format::Format_A2BGR30_Premultiplied:
        return "A2BGR30 (premultiplied)";
    case QImage::Format::Format_A2RGB30_Premultiplied:
        return "A2RGB30 (premultiplied)";
    case QImage::Format::Format_ARGB32:
        return "ARGB32";
    case QImage::Format::Format_ARGB32_Premultiplied:
        return "ARGB32 (premultiplied)";
    case QImage::Format::Format_ARGB4444_Premultiplied:
        return "ARGB4444 (premultiplied)";
    case QImage::Format::Format_ARGB6666_Premultiplied:
        return "ARGB6666 (premultiplied)";
    case QImage::Format::Format_ARGB8555_Premultiplied:
        return "ARGB8555 (premultiplied)";
    case QImage::Format::Format_ARGB8565_Premultiplied:
        return "ARGB8565 (premultiplied)";
    case QImage::Format::Format_Alpha8:
        return "Alpha8";
    case QImage::Format::Format_BGR30:
        return "BGR30";
    case QImage::Format::Format_Grayscale8:
        return "Grayscale8";
    case QImage::Format::Format_Indexed8:
        return "Indexed8";
    case QImage::Format::Format_Invalid:
        return "Invalid";
    case QImage::Format::Format_Mono:
        return "Mono";
    }

    return tr("unknown>");
}

void MainWindow::AddInfoLineStr(const QString &name, const QString &value)
{
    AddInfoLine(name + ":\t" + value);
}

void MainWindow::AddInfoLineBool(const QString &name, bool value)
{
    if (value)
    {
        AddInfoLineStr(name, tr("Yes"));
    }
    else
    {
        AddInfoLineStr(name, tr("No"));
    }
}

void MainWindow::AddInfoLineInt(const QString &name, int64_t value)
{
    AddInfoLineStr(name, QString("%1").arg(value));
}

void MainWindow::AddInfoLine(const QString &line)
{
    QString text = ui->textBrowserClipboardInfo->toPlainText();

    if (!text.isEmpty())
    {
        text += "\n";
    }

    text += line;

    ui->textBrowserClipboardInfo->setText(text);
}
