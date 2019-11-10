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
  QAction *signMessageAction = nullptr;
  QAction *verifyMessageAction = nullptr;
  QAction *aboutAction = nullptr;
  QAction *optionsAction = nullptr;
  QAction *encryptWalletAction = nullptr;
  QAction *backupWalletAction = nullptr;
  QAction *changePassphraseAction = nullptr;
  QAction *aboutQtAction = nullptr;
  QAction *openAction = nullptr;
  QAction *showHelpMessageAction = nullptr;
  QAction *m_create_wallet_action{nullptr};
  QAction *m_open_wallet_action{nullptr};
  QAction *m_close_wallet_action{nullptr};

  /** Create the main UI actions. */
  void createActions();
  /** Create the menu bar and sub-menus. */
  void createMenuBar();
};
#endif // MAINWINDOW_H
