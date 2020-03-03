#ifndef WALLET_H
#define WALLET_H

#include "sec_block.h"

class Wallet {
public:
  explicit Wallet();

  bool isCrypted();
  bool encryptWallet(const SecureString &wallet_passphrase);
  bool changeWalletPassphrase(const SecureString &old_wallet_passphrase,
                              const SecureString &new_wallet_passphrase);

  bool isLocked();
  bool lock();
  bool unlock(const SecureString &wallet_passphrase);

  /*
  // Note: List all APIs of WalletImpl class in Bitcoin
  explicit Wallet();
  bool encryptWallet(const SecureString &wallet_passphrase);
  bool isCrypted();
  bool lock();
  bool unlock(const SecureString &wallet_passphrase);
  bool isLocked();
  bool changeWalletPassphrase(const SecureString &old_wallet_passphrase,
                              const SecureString &new_wallet_passphrase);
  void abortRescan();
  bool backupWallet(const std::string &filename);
  std::string getWalletName();
  bool getNewDestination(const OutputType type, const std::string label,
                         CTxDestination &dest);
  bool getPubKey(const CScript &script, const CKeyID &address,
                 CPubKey &pub_key);
  bool getPrivKey(const CScript &script, const CKeyID &address, CKey &key);
  bool isSpendable(const CTxDestination &dest);
  bool haveWatchOnly();
  bool setAddressBook(const CTxDestination &dest, const std::string &name,
                      const std::string &purpose);
  bool delAddressBook(const CTxDestination &dest);
  bool getAddress(const CTxDestination &dest, std::string *name,
                  isminetype *is_mine, std::string *purpose);
  std::vector<WalletAddress> getAddresses();
  bool addDestData(const CTxDestination &dest, const std::string &key,
                   const std::string &value);
  bool eraseDestData(const CTxDestination &dest, const std::string &key);
  std::vector<std::string> getDestValues(const std::string &prefix);
  void lockCoin(const COutPoint &output);
  void unlockCoin(const COutPoint &output);
  bool isLockedCoin(const COutPoint &output);
  void listLockedCoins(std::vector<COutPoint> &outputs);
  CTransactionRef createTransaction(const std::vector<CRecipient> &recipients,
                                    const CCoinControl &coin_control, bool sign,
                                    int &change_pos, CAmount &fee,
                                    std::string &fail_reason);
  void commitTransaction(CTransactionRef tx, WalletValueMap value_map,
                         WalletOrderForm order_form);
  bool transactionCanBeAbandoned(const uint256 &txid);
  bool abandonTransaction(const uint256 &txid);
  bool transactionCanBeBumped(const uint256 &txid);
  bool createBumpTransaction(const uint256 &txid,
                             const CCoinControl &coin_control,
                             CAmount total_fee,
                             std::vector<std::string> &errors, CAmount &old_fee,
                             CAmount &new_fee, CMutableTransaction &mtx);
  bool signBumpTransaction(CMutableTransaction &mtx);
  bool commitBumpTransaction(const uint256 &txid, CMutableTransaction &&mtx,
                             std::vector<std::string> &errors,
                             uint256 &bumped_txid);
  CTransactionRef getTx(const uint256 &txid);
  WalletTx getWalletTx(const uint256 &txid);
  std::vector<WalletTx> getWalletTxs();
  bool tryGetTxStatus(const uint256 &txid,
                      interfaces::WalletTxStatus &tx_status, int &num_blocks,
                      int64_t &block_time);
  WalletTx getWalletTxDetails(const uint256 &txid, WalletTxStatus &tx_status,
                              WalletOrderForm &order_form, bool &in_mempool,
                              int &num_blocks);
  TransactionError fillPSBT(PartiallySignedTransaction &psbtx, bool &complete,
                            int sighash_type = 1, // SIGHASH_ALL
                            bool sign = true, bool bip32derivs = false);
  WalletBalances getBalances();
  bool tryGetBalances(WalletBalances &balances, int &num_blocks);
  CAmount getBalance();
  CAmount getAvailableBalance(const CCoinControl &coin_control);
  isminetype txinIsMine(const CTxIn &txin);
  isminetype txoutIsMine(const CTxOut &txout);
  CAmount getDebit(const CTxIn &txin, isminefilter filter);
  CAmount getCredit(const CTxOut &txout, isminefilter filter);
  CoinsList listCoins();
  std::vector<WalletTxOut> getCoins(const std::vector<COutPoint> &outputs);
  CAmount getRequiredFee(unsigned int tx_bytes);
  CAmount getMinimumFee(unsigned int tx_bytes, const CCoinControl &coin_control,
                        int *returned_target, FeeReason *reason);
  unsigned int getConfirmTarget();
  bool hdEnabled();
  bool canGetAddresses();
  bool IsWalletFlagSet(uint64_t flag);
  OutputType getDefaultAddressType();
  OutputType getDefaultChangeType();
  CAmount getDefaultMaxTxFee();
  void remove();
  std::unique_ptr<Handler> handleUnload(UnloadFn fn);
  std::unique_ptr<Handler> handleShowProgress(ShowProgressFn fn);
  std::unique_ptr<Handler> handleStatusChanged(StatusChangedFn fn);
  std::unique_ptr<Handler> handleAddressBookChanged(AddressBookChangedFn fn);
  std::unique_ptr<Handler> handleTransactionChanged(TransactionChangedFn fn);
  std::unique_ptr<Handler> handleWatchOnlyChanged(WatchOnlyChangedFn fn);
  std::unique_ptr<Handler>
  handleCanGetAddressesChanged(CanGetAddressesChangedFn fn);
  */
};

#endif // WALLET_H
