#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Create actions for the toolbar, menu bar and tray/dock icon
  createActions();

  // Create application menu bar
  createMenuBar();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::createActions() {
  quitAction = new QAction(tr("E&xit"), this);
  quitAction->setStatusTip(tr("Quit application"));
  quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
  quitAction->setMenuRole(QAction::QuitRole);
  aboutAction = new QAction(tr("&About me"), this);
  aboutAction->setStatusTip(tr("Show information about me"));
  aboutAction->setMenuRole(QAction::AboutRole);
  optionsAction = new QAction(tr("&Options"), this);
  optionsAction->setStatusTip(tr("Modify configuration options"));
  optionsAction->setMenuRole(QAction::PreferencesRole);

  encryptWalletAction = new QAction(tr("&Encrypt Wallet"), this);
  encryptWalletAction->setStatusTip(
      tr("Encrypt the private keys that belong to your wallet"));
  encryptWalletAction->setCheckable(true);
  backupWalletAction = new QAction(tr("&Backup Wallet"), this);
  backupWalletAction->setStatusTip(tr("Backup wallet to another location"));
  changePassphraseAction = new QAction(tr("&Change Passphrase"), this);
  changePassphraseAction->setStatusTip(
      tr("Change the passphrase used for wallet encryption"));

  openWalletAction = new QAction(tr("Open Wallet"), this);
  openWalletAction->setStatusTip(tr("Open a wallet"));

  closeWalletAction = new QAction(tr("Close Wallet"), this);
  closeWalletAction->setStatusTip(tr("Close wallet"));

  createWalletAction = new QAction(tr("Create Wallet"), this);
  createWalletAction->setStatusTip(tr("Create a new wallet"));

  showHelpAction = new QAction(tr("&Command-line options"), this);
  showHelpAction->setMenuRole(QAction::NoRole);
  showHelpAction->setStatusTip(tr("Show the help message to get a list with "
                                  "possible command-line options"));

  connect(quitAction, &QAction::triggered, qApp, QApplication::quit);
}

void MainWindow::createMenuBar() {
#ifdef Q_OS_MAC
  // Create a decoupled menu bar on Mac which stays even if the window is closed
  appMenuBar = new QMenuBar();
#else
  // Get the main window's menu bar on other platforms
  appMenuBar = menuBar();
#endif

  // Configure the menus
  QMenu *file = appMenuBar->addMenu(tr("&File"));
  file->addAction(createWalletAction);
  file->addAction(openWalletAction);
  file->addAction(closeWalletAction);
  file->addSeparator();
  file->addAction(backupWalletAction);
  file->addSeparator();
  file->addAction(quitAction);

  QMenu *settings = appMenuBar->addMenu(tr("&Settings"));
  settings->addAction(encryptWalletAction);
  settings->addAction(changePassphraseAction);
  settings->addSeparator();
  settings->addAction(optionsAction);

  QMenu *help = appMenuBar->addMenu(tr("&Help"));
  help->addAction(showHelpAction);
  help->addSeparator();
  help->addAction(aboutAction);
}
