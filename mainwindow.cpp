#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Create actions for the toolbar, menu bar and tray/dock icon
  // Needs walletFrame to be initialized
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
  aboutAction->setEnabled(false);
  aboutQtAction = new QAction(tr("About &Qt"), this);
  aboutQtAction->setStatusTip(tr("Show information about Qt"));
  aboutQtAction->setMenuRole(QAction::AboutQtRole);
  optionsAction = new QAction(tr("&Options..."), this);
  optionsAction->setStatusTip(tr("Modify configuration options"));
  optionsAction->setMenuRole(QAction::PreferencesRole);
  optionsAction->setEnabled(false);

  encryptWalletAction = new QAction(tr("&Encrypt Wallet..."), this);
  encryptWalletAction->setStatusTip(
      tr("Encrypt the private keys that belong to your wallet"));
  encryptWalletAction->setCheckable(true);
  backupWalletAction = new QAction(tr("&Backup Wallet..."), this);
  backupWalletAction->setStatusTip(tr("Backup wallet to another location"));
  changePassphraseAction = new QAction(tr("&Change Passphrase..."), this);
  changePassphraseAction->setStatusTip(
      tr("Change the passphrase used for wallet encryption"));
  backupWalletAction = new QAction(tr("&Backup Wallet..."), this);
  backupWalletAction->setStatusTip(tr("Backup wallet to another location"));
  signMessageAction = new QAction(tr("Sign &message..."), this);
  signMessageAction->setStatusTip(
      tr("Sign messages with your Bitcoin addresses to prove you own them"));
  verifyMessageAction = new QAction(tr("&Verify message..."), this);
  verifyMessageAction->setStatusTip(
      tr("Verify messages to ensure they were signed with specified Bitcoin "
         "addresses"));

  openAction = new QAction(tr("Open &URI..."), this);
  openAction->setStatusTip(tr("Open a bitcoin: URI"));

  m_open_wallet_action = new QAction(tr("Open Wallet"), this);
  m_open_wallet_action->setEnabled(false);
  m_open_wallet_action->setStatusTip(tr("Open a wallet"));

  m_close_wallet_action = new QAction(tr("Close Wallet..."), this);
  m_close_wallet_action->setStatusTip(tr("Close wallet"));

  m_create_wallet_action = new QAction(tr("Create Wallet..."), this);
  m_create_wallet_action->setStatusTip(tr("Create a new wallet"));

  showHelpMessageAction = new QAction(tr("&Command-line options"), this);
  showHelpMessageAction->setMenuRole(QAction::NoRole);
  showHelpMessageAction->setStatusTip(tr("Show the help message to get a list "
                                         "with possible command-line options"));

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
  file->addAction(m_create_wallet_action);
  file->addAction(m_open_wallet_action);
  file->addAction(m_close_wallet_action);
  file->addSeparator();
  file->addAction(openAction);
  file->addAction(backupWalletAction);
  file->addAction(signMessageAction);
  file->addAction(verifyMessageAction);
  file->addSeparator();
  file->addAction(quitAction);

  QMenu *settings = appMenuBar->addMenu(tr("&Settings"));
  settings->addAction(encryptWalletAction);
  settings->addAction(changePassphraseAction);
  settings->addSeparator();
  settings->addAction(optionsAction);

  QMenu *help = appMenuBar->addMenu(tr("&Help"));
  help->addAction(showHelpMessageAction);
  help->addSeparator();
  help->addAction(aboutAction);
  help->addAction(aboutQtAction);
}
