#ifndef MDCWINDOW_H
#define MDCWINDOW_H

#include <QMainWindow>
#include <qgraphicsscene.h>

namespace Ui
{
    class MDCWindow;
}

class MDCWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
        explicit MDCWindow(QWidget *parent = 0);
        ~MDCWindow();

        /**
         * Draws the screen based on the data for the currently-loaded level.
         */
        void drawScreen();

    private slots :

        /**
         * This is the action taken when File > Open ROM is accessed.
         * This shows the user an open file dialog filtered to .smc and .sfc, and if
         * the user opens a file, the file is then validated based on size. If
         * successful, the globals for the currently opened file and header size are
         * set, and level 0 is opened by default.
         */
        void actionOpen_ROM();

        /**
         * Not implemented yet.
         */
        void actionSave_ROM();

        /**
         * Opens a level chosen by the user.
         */
        void actionLoad_Level();

        /**
         * Opens the level previous to this one.
         */
        void decLevel();

        /**
         * Opens the level after this one.
         */
        void incLevel();
    
    private:
        Ui::MDCWindow *ui;
        QGraphicsScene *scene;
        QPixmap TileGFX[0x1B];
};

#endif // MDCWINDOW_H
