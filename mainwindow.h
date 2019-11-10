#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

  QMenuBar *appMenuBar = nullptr;
  QAction *quitAction = nullptr;
  QAction *aboutAction = nullptr;
  QAction *optionsAction = nullptr;
  QAction *encryptWalletAction = nullptr;
  QAction *backupWalletAction = nullptr;
  QAction *changePassphraseAction = nullptr;
  QAction *showHelpAction = nullptr;
  QAction *createWalletAction = nullptr;
  QAction *openWalletAction = nullptr;
  QAction *closeWalletAction = nullptr;

  /** Create the main UI actions. */
  void createActions();
  /** Create the menu bar and sub-menus. */
  void createMenuBar();
};
#endif // MAINWINDOW_H
