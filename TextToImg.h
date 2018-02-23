#ifndef TEXTTOIMG_H
#define TEXTTOIMG_H

#include <QMainWindow>
#include <QActionGroup>
#include <QAction>
#include <QFontComboBox>
#include <QStringListModel>
#include <QCompleter>
#include <QSpinBox>
#include <QColorDialog>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

namespace Ui {
class TextToImg;
}

class TextToImg : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextToImg(QWidget *parent = 0);
    ~TextToImg();

private slots:
    void on_genImgAct_triggered();
    void setAlignment(QAction *act);
    void setCurrentFontFamily(const QString &text);
    void setFontSize(int i);
    void setBG(int i);
    void setBGColor();
    void setTextColor();
    void setToolBarIcon(QAction *act);
    void onTextChange();

private:
    Ui::TextToImg *ui;
    QActionGroup *actGrp;
    void genImg();
    int flags;
    QFontComboBox *fontComboBox;
    QFont font;
    QSpinBox *spinBox;
    QStringList getAllFont();
    QComboBox *BGComboBox;
    int alpha;
    QAction *BGColorAct;
    QAction *textColorAct;
    QColor color;
    QColor BGColor;
    void setSimpleIcon();
    void setClassicIcon();
    QActionGroup *toolBarGrp;
    void writeINI();
    void readINI();
    int simple, classic;
};

#endif // TEXTTOIMG_H
