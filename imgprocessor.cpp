#include "imgprocessor.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <Qapplication>
#include <QColorDialog>
#include <QColor>
#include <QTextList>
ImgProcessor::ImgProcessor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Easy Word");

    showWidget = new ShowWidget(this);
    setCentralWidget(showWidget);


    fontLabel1 = new QLabel("字体：");
    fontComboBox = new QFontComboBox;
    fontComboBox->setFontFilters(QFontComboBox::ScalableFonts);

    fontLabel2 = new QLabel("字号");
    sizeComboBox = new QComboBox;
    QFontDatabase db;
    foreach (int size, db.standardSizes()) {
        sizeComboBox->addItem(QString::number(size));
    }

    boldBtn = new QToolButton;
    //boldBtn->setIcon(QIcon(":/1.png"));
    boldBtn->setCheckable(true);
    italicBtn = new QToolButton;
    italicBtn->setCheckable(true);

    underlineBtn = new QToolButton;
    //underlineBtn->setIcon(QIcon(":/1.png"));
    underlineBtn->setCheckable(true);

    colorBtn = new QToolButton;
    //colorBtn->setIcon(QIcon(":/1.png"));
    colorBtn->setCheckable(true);

    listLabel = new QLabel("排序");
    listComboBox = new QComboBox;
    listComboBox->addItem("Standard");
    listComboBox->addItem("QTextListFormat::ListDisc");
    listComboBox->addItem("QTextListFormat::ListCircle");
    listComboBox->addItem("QTextListFormat::ListSquare");
    listComboBox->addItem("QTextListFormat::ListDecimal");
    listComboBox->addItem("QTextListFormat::ListLowerAlpha");
    listComboBox->addItem("QTextListFormat::ListUpperAlpha");
    listComboBox->addItem("QTextListFormat::ListLowerRoman");
    listComboBox->addItem("QTextListFormat::ListUpperRoman");

    connect(listComboBox,SIGNAL(activated(int)),this,SLOT(ShowList(int)));
//    connect(showWidget->text->document(),SIGNAL(undoAvailable(bool)),redoAction,SLOT(setEnabled(bool)));
//    connect(showWidget->text->document(),SIGNAL(redoAvailable(bool)),redoAction,SLOT(setEnabled(bool)));
    connect(showWidget->text,SIGNAL(cursorPositionChanged()),this,SLOT(ShowCursorPositionChanged()));


    connect(fontComboBox,SIGNAL(activated(QString)),this,SLOT(ShowFontComboBox(QString)));
    connect(sizeComboBox,SIGNAL(activated(QString)),this,SLOT(ShowSizeSpinBox(QString)));
    connect(boldBtn,SIGNAL(clicked(bool)),this,SLOT(ShowBoldBtn()));
    connect(italicBtn,SIGNAL(clicked(bool)),this,SLOT(ShowItalicBtn()));
    connect(underlineBtn,SIGNAL(clicked(bool)),this,SLOT(ShowUnderlineBtn()));
    connect(colorBtn,SIGNAL(clicked(bool)),this,SLOT(ShowColorBtn()));
    connect(showWidget->text,SIGNAL(currentCharFormatChanged(QTextCharFormat&)),this,SLOT(ShowCurrentFormatChanged(QTextCharFormat&)));



    createActions();
    createMenus();
    createToolBars();
    if(img.load(":/image.bmp"))
    {
        showWidget->imageLabel->setPixmap(QPixmap::fromImage(img));
    }
    else showWidget->imageLabel->setText("图片加载失败");
}

ImgProcessor::~ImgProcessor()
{

}

void ImgProcessor::loadFile(QString filename)
{
    printf("file name:%s\n",filename.data());

    QFile file(filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream textStream(&file);
        while(!textStream.atEnd())
        {
            showWidget->text->append(textStream.readLine());
            printf("read line\n");
        }
        printf("end\n");
    }
}

void ImgProcessor::ShowOpenFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        if(showWidget->text->document()->isEmpty())
        {
            loadFile(fileName);
        }
        else
        {
            ImgProcessor *newImgProcessor = new ImgProcessor;
            newImgProcessor->show();
            newImgProcessor->loadFile(fileName);
        }
    }
}

void ImgProcessor::ShowNewFile()
{
    ImgProcessor *newImgProcessor = new ImgProcessor;
    newImgProcessor->show();
}

void ImgProcessor::ShowPrintText()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer,this);
    if(printDialog.exec())
    {
        QTextDocument *doc = showWidget->text->document();
        doc->print(&printer);
    }
}

void ImgProcessor::ShowPrintImage()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer,this);
    if(printDialog.exec())
    {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = img.size();
        size.scale(rect.size(),Qt::KeepAspectRatio);

        painter.setViewport(rect.x(),rect.y(),size.width(),size.height());
        painter.setWindow(img.rect());
        painter.drawImage(0,0,img);
    }
}

void ImgProcessor::ShowZoomIn()
{
    if(img.isNull())
        return;
    QMatrix matrix;
    matrix.scale(2,2);
    img = img.transformed(matrix);
    showWidget->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void ImgProcessor::ShowZoomOut()
{
    if(img.isNull())
        return;
    QMatrix matrix;
    matrix.scale(0.5,0.5);
    img = img.transformed(matrix);
    showWidget->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void ImgProcessor::ShowRotate90()
{
    if(img.isNull())
        return;
    QMatrix matrix;
    matrix.rotate(90);
    img = img.transformed(matrix);
    showWidget->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void ImgProcessor::ShowRotate180()
{
    if(img.isNull())
        return;
    QMatrix matrix;
    matrix.rotate(180);
    img = img.transformed(matrix);
    showWidget->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void ImgProcessor::ShowRotate270()
{
    if(img.isNull())
        return;
    QMatrix matrix;
    matrix.rotate(270);
    img = img.transformed(matrix);
    showWidget->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void ImgProcessor::ShowMirrorVertical()
{
    if(img.isNull())
        return;
    img = img.mirrored(false,true);
    showWidget->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void ImgProcessor::ShowMirrorHorizontal()
{
    if(img.isNull())
        return;
    img = img.mirrored(true,false);
    showWidget->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void ImgProcessor::createActions()
{
    openFileAction = new QAction("打开",this);
    openFileAction->setShortcut(tr("Ctrl+O"));
    openFileAction->setStatusTip("打开一个文件");
    connect(openFileAction,SIGNAL(triggered(bool)),this,SLOT(ShowOpenFile()));

    NewFileAction = new QAction("新建",this);
    NewFileAction->setShortcut(tr("Ctrl+N"));
    NewFileAction->setStatusTip("新建一个文件");
    connect(NewFileAction,SIGNAL(triggered()),this,SLOT(ShowNewFile()));

    exitAction = new QAction("退出",this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip("复制文件");
    connect(exitAction,SIGNAL(triggered()),showWidget->text,SLOT(close()));

    copyAction = new QAction("复制",this);
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setStatusTip("复制文件");
    connect(copyAction,SIGNAL(triggered()),showWidget->text,SLOT(copy()));

    cutAction = new QAction("剪切",this);
    cutAction->setShortcut(tr("Ctrl+X"));
    cutAction->setStatusTip("剪切文件");
    connect(cutAction,SIGNAL(triggered()),showWidget->text,SLOT(cut()));

    pasteAction = new QAction("粘贴",this);
    pasteAction->setShortcut(tr("Ctrl+V"));
    pasteAction->setStatusTip("粘贴文件");
    connect(pasteAction,SIGNAL(triggered()),showWidget->text,SLOT(paste()));

    aboutAction = new QAction("关于",this);
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(Qapplication::aboutQt()));

    PrintTextAction = new QAction("打印文本",this);
    PrintTextAction->setStatusTip("打印一个文本");
    connect(PrintTextAction,SIGNAL(triggered(bool)),this,SLOT(ShowPrintText()));

    PrintImageAction = new QAction("打印图像",this);
    PrintImageAction->setStatusTip("打印一幅图像");
    connect(PrintImageAction,SIGNAL(triggered(bool)),this,SLOT(ShowPrintImage()));

    zoomInAction = new QAction("放大",this);
    zoomInAction->setStatusTip("放大一张图片");
    connect(zoomInAction,SIGNAL(triggered(bool)),this,SLOT(ShowZoomIn()));

    zoomOutAction = new QAction("缩小",this);
    zoomOutAction->setStatusTip("缩小一张图片");
    connect(zoomOutAction,SIGNAL(triggered(bool)),this,SLOT(ShowZoomOut()));

    rotate90Action = new QAction("旋转90°",this);
    rotate90Action->setStatusTip("将一幅图旋转90°");
    connect(rotate90Action,SIGNAL(triggered(bool)),this,SLOT(ShowRotate90()));

    rotate180Action = new QAction("旋转180°",this);
    rotate180Action->setStatusTip("将一幅图旋转180°");
    connect(rotate180Action,SIGNAL(triggered(bool)),this,SLOT(ShowRotate180()));

    rotate270Action = new QAction("旋转270°",this);
    rotate270Action->setStatusTip("将一幅图旋转270°");
    connect(rotate270Action,SIGNAL(triggered(bool)),this,SLOT(ShowRotate270()));

    mirrorVerticalAction = new QAction("纵向镜像",this);
    mirrorVerticalAction->setStatusTip("对一张图做纵向镜像");
    connect(mirrorVerticalAction,SIGNAL(triggered(bool)),this,SLOT(ShowMirrorVertical()));

    mirrorHorizontalAction = new QAction("横向镜像",this);
    mirrorHorizontalAction->setStatusTip("对一张图做横向镜像");
    connect(mirrorHorizontalAction,SIGNAL(triggered(bool)),this,SLOT(ShowMirrorHorizontal()));

    undoAction = new QAction("撤销",this);
    connect(undoAction,SIGNAL(triggered()),showWidget->text,SLOT(undo()));

    redoAction = new QAction("重做",this);
    connect(redoAction,SIGNAL(triggered()),showWidget->text,SLOT(redo()));


    actGrp = new QActionGroup(this);
    leftAction = new QAction("左对齐",actGrp);
    leftAction->setCheckable(true);

    rightAction = new QAction("右对齐",actGrp);
    rightAction->setCheckable(true);

    centerAction = new QAction("居中",actGrp);
    centerAction->setCheckable(true);
    justifyAction = new QAction("两端对齐",actGrp);
    justifyAction->setCheckable(true);

    connect(actGrp,SIGNAL(triggered(QAction*)),this,SLOT(ShowAlignment(QAction*)));
}

void ImgProcessor::createMenus()
{
    fileMenu = menuBar()->addMenu("文件");
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(NewFileAction);
    fileMenu->addAction(PrintTextAction);
    fileMenu->addAction(PrintImageAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    zoomMenu = menuBar()->addMenu("编辑");
    zoomMenu->addAction(copyAction);
    zoomMenu->addAction(cutAction);
    zoomMenu->addAction(pasteAction);
    zoomMenu->addAction(aboutAction);
    zoomMenu->addSeparator();
    zoomMenu->addAction(zoomInAction);
    zoomMenu->addAction(zoomOutAction);

    rotateMenu = menuBar()->addMenu("旋转");
    rotateMenu->addAction(rotate90Action);
    rotateMenu->addAction(rotate180Action);
    rotateMenu->addAction(rotate270Action);

    mirrorMenu = menuBar()->addMenu("镜像");
    mirrorMenu->addAction(mirrorVerticalAction);
    mirrorMenu->addAction(mirrorHorizontalAction);

}

void ImgProcessor::createToolBars()
{
    fileTool = addToolBar("File");
    fileTool->addAction(openFileAction);
    fileTool->addAction(NewFileAction);
    fileTool->addAction(PrintTextAction);
    fileTool->addAction(PrintImageAction);

    zoomTool = addToolBar("Edit");
    zoomTool->addAction(copyAction);
    zoomTool->addAction(cutAction);
    zoomTool->addAction(pasteAction);
    zoomTool->addSeparator();
    zoomTool->addAction(zoomInAction);
    zoomTool->addAction(zoomOutAction);

    rotateTool = addToolBar("rotate");
    rotateTool->addAction(rotate90Action);
    rotateTool->addAction(rotate180Action);
    rotateTool->addAction(rotate270Action);

    doToolBar = addToolBar("doEdit");
    doToolBar->addAction(undoAction);
    doToolBar->addAction(redoAction);

    fontToolBar = addToolBar("Font");

    fontToolBar->addWidget(fontLabel1);
    fontToolBar->addWidget(fontComboBox);
    fontToolBar->addWidget(fontLabel2);
    fontToolBar->addWidget(sizeComboBox);

    fontToolBar->addSeparator();
    fontToolBar->addWidget(boldBtn);
    fontToolBar->addWidget(italicBtn);
    fontToolBar->addWidget(underlineBtn);

    fontToolBar->addSeparator();
    fontToolBar->addWidget(colorBtn);


    fileTool->setMovable(false);


    listToolBar = addToolBar("list");
    listToolBar->addWidget(listLabel);
    listToolBar->addWidget(listComboBox);
    listToolBar->addSeparator();
    listToolBar->addActions(actGrp->actions());
}

void ImgProcessor::ShowFontComboBox(QString comboStr)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(comboStr);
    mergeFormat(fmt);
}
void ImgProcessor::ShowSizeSpinBox(QString spinValue)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(spinValue.toFloat());
    showWidget->text->mergeCurrentCharFormat(fmt);
}
void ImgProcessor::ShowBoldBtn()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(boldBtn->isChecked()?QFont::Bold:QFont::Normal);
    showWidget->text->mergeCurrentCharFormat(fmt);
}
void ImgProcessor::ShowItalicBtn()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italicBtn->isChecked());
    showWidget->text->mergeCurrentCharFormat(fmt);
}
void ImgProcessor::ShowUnderlineBtn()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underlineBtn->isChecked());
    showWidget->text->mergeCurrentCharFormat(fmt);
}
void ImgProcessor::ShowColorBtn()
{
    QColor color = QColorDialog::getColor(Qt::red,this);
    if(color.isValid())
    {
        QTextCharFormat fmt;
        fmt.setForeground(color);
        showWidget->text->mergeCurrentCharFormat(fmt);
    }
}
void ImgProcessor::ShowCurrentFormatChanged(const QTextCharFormat &fmt)
{
    fontComboBox->setCurrentIndex(fontComboBox->findText(fmt.fontFamily()));
    sizeComboBox->setCurrentIndex(sizeComboBox->findText(QString::number(fmt.fontPointSize())));
    boldBtn->setChecked(fmt.fontItalic());
    underlineBtn->setChecked(fmt.fontUnderline());
}

void ImgProcessor::mergeFormat(QTextCharFormat format)
{
    QTextCursor cursor = showWidget->text->textCursor();
    if(!cursor.hasSelection())
    {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    cursor.mergeCharFormat(format);
    showWidget->text->mergeCurrentCharFormat(format);
}

void ImgProcessor::ShowList(int index)
{
    QTextCursor cursor = showWidget->text->textCursor();

    if(index!=0)
    {
        QTextListFormat::Style style = QTextListFormat::ListDisc;
        switch(index)
        {
        default:
        case 1:
            style = QTextListFormat::ListDisc;
            break;
        case 2:
            style = QTextListFormat::ListCircle;
            break;
        case 3:
            style = QTextListFormat::ListSquare;
            break;
        case 4:
            style = QTextListFormat::ListDecimal;
            break;
        case 5:
            style = QTextListFormat::ListLowerAlpha;
            break;
        case 6:
            style = QTextListFormat::ListUpperAlpha;
            break;
        case 7:
            style = QTextListFormat::ListLowerRoman;
            break;
        case 8:
            style = QTextListFormat::ListUpperRoman;
            break;
        }
        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();
        QTextListFormat listFmt;

        if(cursor.currentList())
        {
            listFmt = cursor.currentList()->format();
        }
        else
        {
            listFmt.setIndent(blockFmt.indent()+1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);

        cursor.endEditBlock();
    }
    else
    {
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}
void ImgProcessor::ShowAlignment(QAction *act)
{
    if(act==leftAction)
        showWidget->text->setAlignment(Qt::AlignLeft);
    if(act==rightAction)
        showWidget->text->setAlignment(Qt::AlignRight);
    if(act==centerAction)
        showWidget->text->setAlignment(Qt::AlignCenter);
    if(act==justifyAction)
        showWidget->text->setAlignment(Qt::AlignJustify);
}
void ImgProcessor::ShowCursorPositionChanged()
{
    if(showWidget->text->alignment()==Qt::AlignLeft)
        leftAction->setChecked(true);
    if(showWidget->text->alignment()==Qt::AlignRight)
        rightAction->setChecked(true);
    if(showWidget->text->alignment()==Qt::AlignCenter)
        centerAction->setChecked(true);
    if(showWidget->text->alignment()==Qt::AlignJustify)
        justifyAction->setChecked(true);
}
