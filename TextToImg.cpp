#include "TextToImg.h"
#include "ui_TextToImg.h"

TextToImg::TextToImg(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextToImg)
{
    ui->setupUi(this);

    QDir dir = QDir::currentPath();
    QString imagePath = dir.absoluteFilePath("images");
    QDir imageDir(imagePath);
    if(!imageDir.exists())
        dir.mkdir("images");

    //文字位置
    actGrp = new QActionGroup(this);
    actGrp->addAction(ui->leftAct);
    actGrp->addAction(ui->centerAct);
    actGrp->addAction(ui->rightAct);
    actGrp->addAction(ui->justifyAct);
    ui->mainToolBar->addActions(actGrp->actions());
    flags = Qt::AlignCenter;

    //文字加粗/斜体/下划线
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->boldAct);
    ui->mainToolBar->addAction(ui->italicAct);
    ui->mainToolBar->addAction(ui->underAct);
    ui->mainToolBar->addSeparator();

    //文字字体、字号设置
    font.setFamily(QString("SimSun"));
    fontComboBox = new QFontComboBox(this);
    fontComboBox->setFontFilters(QFontComboBox::ScalableFonts);
    fontComboBox->setCurrentFont(font);
    QLabel *fontFamilyLabel = new QLabel(tr("字体：") ,this);
    ui->fontToolBar->addWidget(fontFamilyLabel);
    ui->fontToolBar->addWidget(fontComboBox);
    ui->fontToolBar->addSeparator();
    spinBox = new QSpinBox(this);
    spinBox->setRange(1, 100);
    spinBox->setValue(100);
    spinBox->setAlignment(Qt::AlignCenter);
    font.setPixelSize(spinBox->value());
    spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    QLabel *fontSizeLabel = new QLabel(tr("字号：") ,this);
    ui->fontToolBar->addWidget(fontSizeLabel);
    ui->fontToolBar->addWidget(spinBox);

    //字体列表显示输入提示
    QStringList list = getAllFont();
    QStringListModel *model = new QStringListModel;
    model->setStringList(list);
    QCompleter *completer = new QCompleter(model);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    fontComboBox->setCompleter(completer);

    //图片背景透明/不透明
    BGComboBox = new QComboBox(this);
    BGComboBox->addItem(tr("透明"));
    BGComboBox->addItem(tr("不透明"));
    QLabel *BGLabel = new QLabel(tr("图片背景："), this);
    ui->mainToolBar->addWidget(BGLabel);
    ui->mainToolBar->addWidget(BGComboBox);
    alpha = BGComboBox->currentIndex();
    ui->mainToolBar->addSeparator();
    BGColorAct = new QAction(tr("&背景颜色"), this);
    textColorAct = new QAction(tr("&文字颜色"), this);
    ui->mainToolBar->addAction(BGColorAct);
    ui->mainToolBar->addSeparator();
    alpha == 0 ? BGColorAct->setEnabled(false) : BGColorAct->setEnabled(true);
    ui->mainToolBar->addAction(textColorAct);
    color = QColor(Qt::black);
    BGColor = QColor(Qt::white);
    ui->editMenu->addAction(BGColorAct);
    ui->editMenu->addAction(textColorAct);
    ui->editMenu->addSeparator();
    ui->editMenu->addAction(ui->quitAct);

    //设置图标
    toolBarGrp = new QActionGroup(this);
    toolBarGrp->addAction(ui->simpleAct);
    toolBarGrp->addAction(ui->classicAct);
    ui->toolaBarMenu->addActions(toolBarGrp->actions());

    if(QFile::exists("icon.ini"))
        readINI();
    else {
        simple = 0;
        classic = 1;
        writeINI();
    }
    simple == 0 ? setSimpleIcon() : setClassicIcon();
    simple == 0 ? ui->simpleAct->setChecked(true)
                : ui->classicAct->setChecked(true);

    connect(actGrp, &QActionGroup::triggered, this, &TextToImg::setAlignment);
    connect(ui->boldAct, &QAction::triggered, [this](bool checked){font.setBold(checked);genImg();});
    connect(ui->italicAct, &QAction::triggered, [this](bool checked){font.setItalic(checked);genImg();});
    connect(ui->underAct, &QAction::triggered, [this](bool checked){font.setUnderline(checked);genImg();});
    connect(fontComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setCurrentFontFamily(QString)));
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setFontSize(int)));
    connect(BGComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBG(int)));
    connect(BGColorAct, &QAction::triggered, this, &TextToImg::setBGColor);
    connect(textColorAct, &QAction::triggered, this, &TextToImg::setTextColor);
    connect(toolBarGrp, &QActionGroup::triggered, this, &TextToImg::setToolBarIcon);
    connect(ui->quitAct, &QAction::triggered, this, &TextToImg::close);
//    connect(ui->textEdit, &QTextEdit::textChanged, this, &TextToImg::onTextChange);
}

TextToImg::~TextToImg()
{
    delete ui;
}

//获取系统字体列表
QStringList TextToImg::getAllFont()
{
    int count = fontComboBox->count();
    QStringList list;
    for(int i = 0; i < count; i++)
        list << fontComboBox->itemText(i);
    return list;
}

//生成图片
void TextToImg::genImg()
{
    QString imageText = ui->textEdit->toPlainText();
    if(imageText.isEmpty())
        return;

    QFontMetrics fm(font);

    int fontWidth = 0;
    int fontHeight = fm.lineSpacing() * (imageText.count("\n") + 1);
    if(imageText.contains("\n")) {
        QStringList list = imageText.split("\n");
        foreach (QString str, list) {
            int k = fm.boundingRect(str).width();
            if(k > fontWidth)
                fontWidth = k;
        }
    } else
        fontWidth = fm.width(imageText);

    QSize size(fontWidth, fontHeight);

    //以ARGB32格式构造一个QImage
    QImage image(size, QImage::Format_ARGB32);
    //填充图片背景,120/250为透明度
    image.fill(qRgba(BGColor.red(), BGColor.green(), BGColor.blue(), alpha));

    //为这个QImage构造一个QPainter
    QPainter painter(&image);

    //设置画刷的组合模式CompositionMode_SourceOut这个模式为目标图像在上。
    //改变组合模式和上面的填充方式可以画出透明的图片。
    alpha == 0 ? painter.setCompositionMode(QPainter::CompositionMode_DestinationOver)
               : painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

    //设置画笔和字体
    painter.setPen(color);
    painter.setFont(font);

    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);
    //将文字写进image
    painter.drawText(image.rect(), flags, imageText);
//    painter.drawRoundRect(image.rect(), fontWidth / 4, fontHeight / 4);

    QString imgPath(QString("%1/images/%2_%3.png")
                    .arg(QDir::currentPath())
                    .arg(imageText)
                    .arg(QDateTime::currentDateTime().toTime_t()));
    image.save(imgPath, "PNG", 100);

    ui->showLabel->setPixmap(QPixmap::fromImage(image));
}

void TextToImg::on_genImgAct_triggered()
{
    genImg();
}

//设置文字位置
void TextToImg::setAlignment(QAction *act)
{
    if(act == ui->leftAct)
        flags = Qt::AlignLeft;
    if(act == ui->rightAct)
        flags = Qt::AlignRight;
    if(act == ui->centerAct)
        flags = Qt::AlignCenter;
    if(act == ui->justifyAct)
        flags = Qt::AlignJustify;
    genImg();
}

//设置字体
void TextToImg::setCurrentFontFamily(const QString &text)
{
    text.isEmpty() ? font.setFamily(tr("SimSun"))
                   : (getAllFont().contains(text, Qt::CaseInsensitive) ?
                          font.setFamily(text) : font.setFamily(tr("SimSun")));
    genImg();
}

//设置字号
void TextToImg::setFontSize(int i)
{
    i == 0 ? font.setPixelSize(50) : font.setPixelSize(i);
    genImg();
}

//设置背景透明/不透明
void TextToImg::setBG(int i)
{
    i == 0 ? alpha = 0 : alpha = 255;
    i == 0 ? BGColorAct->setEnabled(false) : BGColorAct->setEnabled(true);
    genImg();
}

//设置文字颜色
void TextToImg::setTextColor()
{
    if(ui->textEdit->toPlainText().isEmpty())
        return;
    color = QColorDialog::getColor(color, this, tr("选择文字颜色"));
    if(!color.isValid())
        return;

    genImg();
}

//设置背景颜色
void TextToImg::setBGColor()
{
    if(ui->textEdit->toPlainText().isEmpty())
        return;
    QColor bgColor = QColorDialog::getColor(BGColor, this, tr("选择背景颜色"));
    if(!bgColor.isValid())
        return;

    BGColor = bgColor;
    genImg();
}

//简约图标
void TextToImg::setSimpleIcon()
{
    ui->leftAct->setIcon(QIcon(":/simple/simple_images/format_align_left.ico"));
    ui->rightAct->setIcon(QIcon(":/simple/simple_images/format_align_right.ico"));
    ui->centerAct->setIcon(QIcon(":/simple/simple_images/format_align_center.ico"));
    ui->justifyAct->setIcon(QIcon(":/simple/simple_images/format_align_justify.ico"));
    ui->boldAct->setIcon(QIcon(":/simple/simple_images/format_bold.ico"));
    ui->italicAct->setIcon(QIcon(":/simple/simple_images/format_ital.ico"));
    ui->underAct->setIcon(QIcon(":/simple/simple_images/format_underline.ico"));
    textColorAct->setIcon(QIcon(":/simple/simple_images/format_color_text.ico"));
    BGColorAct->setIcon(QIcon(":/simple/simple_images/color_palette.ico"));
    ui->genImgAct->setIcon(QIcon(":/simple/simple_images/image.ico"));
    ui->quitAct->setIcon(QIcon(":/simple/simple_images/close.ico"));
}


//经典图标
void TextToImg::setClassicIcon()
{
    ui->leftAct->setIcon(QIcon(":/word/word_images/align_left.ico"));
    ui->rightAct->setIcon(QIcon(":/word/word_images/align_right.ico"));
    ui->centerAct->setIcon(QIcon(":/word/word_images/align_center.ico"));
    ui->justifyAct->setIcon(QIcon(":/word/word_images/align_justify.ico"));
    ui->boldAct->setIcon(QIcon(":/word/word_images/bold.ico"));
    ui->italicAct->setIcon(QIcon(":/word/word_images/italic.ico"));
    ui->underAct->setIcon(QIcon(":/word/word_images/underline.ico"));
    textColorAct->setIcon(QIcon(":/word/word_images/text_color.ico"));
    BGColorAct->setIcon(QIcon(":/word/word_images/background_color.ico"));
    ui->genImgAct->setIcon(QIcon(":/word/word_images/image.ico"));
    ui->quitAct->setIcon(QIcon(":/word/word_images/exit.ico"));
}

//设置图标风格
void TextToImg::setToolBarIcon(QAction *act)
{
    if(act == ui->simpleAct) {
        setSimpleIcon();
        if(simple != 0) {
            simple = 0;
            classic = 1;
            writeINI();
        }
    } else {
        setClassicIcon();
        if(classic != 0) {
            simple = 1;
            classic = 0;
            writeINI();
        }
    }
}

void TextToImg::writeINI()
{
    QSettings settings("icon.ini", QSettings::IniFormat);
    settings.beginGroup(QString("icons"));
    settings.setValue(QString("simple"), simple);
    settings.setValue(QString("classic"), classic);
    settings.endGroup();
}

void TextToImg::readINI()
{
    QSettings settings("icon.ini", QSettings::IniFormat);
    simple = settings.value("icons/simple").toInt();
    classic = settings.value("icons/classic").toInt();
}

void TextToImg::onTextChange()
{
    ui->textEdit->toPlainText().isEmpty() ? ui->showLabel->clear() : genImg();
}
