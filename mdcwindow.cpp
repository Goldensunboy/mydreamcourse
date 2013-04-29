#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qdebug.h>
#include <qgraphicsscene.h>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QRegExp>

#include "mdcwindow.h"
#include "ui_mdcwindow.h"
#include "defs.h"
#include "fileio.h"
#include "levelhandler.h"

MDCWindow::MDCWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MDCWindow), scene(new QGraphicsScene), TileGFX()
{
    ui->setupUi(this);

    // File menu
    connect(ui->actionOpen_ROM, SIGNAL(triggered()), this, SLOT(actionOpen_ROM()));
    connect(ui->actionSave_ROM, SIGNAL(triggered()), this, SLOT(actionSave_ROM()));
    ui->actionSave_ROM->setDisabled(true);
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    // Level menup
    connect(ui->actionLoad_Level, SIGNAL(triggered()), this, SLOT(actionLoad_Level()));
    ui->actionLoad_Level->setDisabled(true);

    // Set up buttons
    connect(ui->OpenFileButton, SIGNAL(clicked()), this, SLOT(actionOpen_ROM()));
    connect(ui->SaveFileButton, SIGNAL(clicked()), this, SLOT(actionSave_ROM()));
    ui->SaveFileButton->setDisabled(true);
    connect(ui->OpenLevelButton, SIGNAL(clicked()), this, SLOT(actionLoad_Level()));
    ui->OpenLevelButton->setDisabled(true);
    connect(ui->DecLevelButton, SIGNAL(clicked()), this, SLOT(decLevel()));
    ui->DecLevelButton->setDisabled(true);
    connect(ui->IncLevelButton, SIGNAL(clicked()), this, SLOT(incLevel()));
    ui->IncLevelButton->setDisabled(true);

    // Set up defaults for graphical elements
    ui->graphicsView->setScene(scene);

    // Create array of hardcoded graphics
    for(int i = 0; i < 0x1B; ++i)
    {
        QString tileStr(":/Resources/Tile");
        if(i + 1 < 0x10)
        {
            tileStr.append('0');
        }
        tileStr.append(QString().setNum(i + 1, 16).toUpper());
        TileGFX[i] = QPixmap::fromImage(QImage(tileStr));
    }
}

MDCWindow::~MDCWindow()
{
    delete ui;
}

void MDCWindow::actionOpen_ROM()
{
    // Get the filepath of a file to open from the user
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open ROM"), "", tr("SNES ROM images (*.smc *.sfc);; All files (*.*)"));

    // If the file was selected, open it
    if(fileName.compare(""))
    {
        // Creae a file object from the filename
        QFile openedFile(fileName);

        // Catch a failed attempt to open the file in read-only mode
        if(!openedFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, "Failed to open ROM!", "The file \"" + fileName + "\" could not be opened!");
            return;
        }

        // Read contents of the file into a byte array
        QByteArray tempFileData(openedFile.readAll());
        openedFile.close();

        // Validate the file size
        QString errorMSG(validateROM(tempFileData));
        if(errorMSG.compare(""))
        {
            QMessageBox::critical(this, "Failed to open ROM!", errorMSG);
            return;
        }

        // Enable all disable controls
        ui->actionSave_ROM->setEnabled(true);
        ui->actionLoad_Level->setEnabled(true);
        ui->SaveFileButton->setEnabled(true);
        ui->OpenLevelButton->setEnabled(true);
        ui->IncLevelButton->setEnabled(true);
        ui->LevelLabel->setText("0");

        // Set globals to the opened data
        file = tempFileData;
        header = file.size() % 0x100000;
        setWindowTitle(QFileInfo(openedFile).fileName().append(" - My Dream Course"));

        // Load level 0 as the default level
        currentLevel = 0;
        loadLevel(currentLevel);

        // Draw the screen after level 0 has been loaded
        drawScreen();
    }
}

void MDCWindow::actionSave_ROM()
{
    qDebug() << "Save ROM\n";
}

void MDCWindow::actionLoad_Level()
{
    // Show the user a prompt for a level to load
    bool ok;
    QString str = QInputDialog::getText(this, "Open Level", "Enter a level number (in hex):", QLineEdit::Normal, QString().setNum(currentLevel, 16).toUpper(), &ok);

    if(ok)
    {
        QRegExp valid("[0-9a-fA-F]{1,2}");
        if(valid.exactMatch(str))
        {
            currentLevel = str.toUInt(&ok, 16);
            loadLevel(currentLevel);
            drawScreen();

            // Enable/disable buttons as needed
            ui->LevelLabel->setText(str.toUpper());
            if(currentLevel > 0)
            {
                ui->DecLevelButton->setEnabled(true);
            }
            else
            {
                ui->DecLevelButton->setDisabled(true);
            }
            if(currentLevel < 0xFF)
            {
                ui->IncLevelButton->setEnabled(true);
            }
            else
            {
                ui->IncLevelButton->setDisabled(true);
            }
        }
        else
        {
            QMessageBox::critical(this, "Invalid input!", "Input a level number in the range 0-FF.");
            actionLoad_Level();
        }
    }
}

void MDCWindow::decLevel()
{
    loadLevel(--currentLevel);
    drawScreen();
    if(currentLevel == 0)
    {
        ui->DecLevelButton->setDisabled(true);
    }
    ui->IncLevelButton->setEnabled(true);
    ui->LevelLabel->setText(QString().setNum(currentLevel, 16).toUpper());
}

void MDCWindow::incLevel()
{
    loadLevel(++currentLevel);
    drawScreen();
    if(currentLevel == 0xFF)
    {
        ui->IncLevelButton->setDisabled(true);
    }
    ui->DecLevelButton->setEnabled(true);
    ui->LevelLabel->setText(QString().setNum(currentLevel, 16).toUpper());
}

void MDCWindow::drawScreen()
{
    // Clear the screen
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

    // Draw the grid for the level
    for(int i = 0; i < levelHeight; ++i)
    {
        for(int j = 0; j < levelWidth; ++j)
        {
            scene->addPixmap(QPixmap::fromImage(QImage(":/Resources/GridLines")))->setPos((i + j)*32, (levelHeight - 1 - i + j)*16);
        }
    }

    // Draw the tiles
    QPixmap bottom = QPixmap::fromImage(QImage(":/Resources/TileBottom"));
    QPixmap spacer = QPixmap::fromImage(QImage(":/Resources/TileSpacer"));
    for(int i = 0; i < levelHeight; ++i)
    {
        for(int j = 0; j < levelWidth; ++j)
        {
            Tile curr = tiles[(levelHeight - i - 1)*levelWidth + j];
            int X = (levelHeight - i - 1 + j)*32;
            int Y = (i + j + 1)*16;

            // Only draw the land if there's a tile
            if(curr.type)
            {
                // Draw the bottom of the land tile
                scene->addPixmap(bottom)->setPos(X, Y);

                // Draw extra height for the tile
                int H = curr.height;
                switch(curr.type) // Reduce drawing height by 1 for selected tiles
                {
                case 0x02:
                case 0x04:
                case 0x05:
                case 0x08:
                case 0x0C:
                case 0x0D:
                case 0x0F:
                case 0x10:
                case 0x14:
                case 0x15:
                case 0x17:
                case 0x18:
                    --H;
                }
                while(H-- > 0)
                {
                    Y -= spacer.height();
                    scene->addPixmap(spacer)->setPos(X, Y);
                }

                // Draw the tile's top
                QPixmap img = TileGFX[curr.type - 1];
                Y -= img.height();
                scene->addPixmap(img)->setPos(X, Y);
            }
        }
    }
}
