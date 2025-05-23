/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_INTERNAL_ORCHARD_SYNC_STATE_H_
#define BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_INTERNAL_ORCHARD_SYNC_STATE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/sequence_checker.h"
#include "brave/components/brave_wallet/browser/internal/orchard_block_scanner.h"
#include "brave/components/brave_wallet/browser/internal/orchard_storage/orchard_shard_tree_types.h"
#include "brave/components/brave_wallet/browser/internal/orchard_storage/orchard_storage.h"
#include "brave/components/brave_wallet/browser/zcash/rust/orchard_shard_tree.h"
#include "brave/components/brave_wallet/common/zcash_utils.h"

namespace brave_wallet {

inline constexpr uint32_t kZCashInternalAddressMinConfirmations = 3u;
inline constexpr uint32_t kZCashPublicAddressMinConfirmations = 10u;

// Represents the persisted synchronization state for the Zcash blockchain.
// The synchronization state includes account-specific information regarding
// spendable and spent notes, sync progress, and the state of the Orchard
// commitment tree, which is used to sign notes for spending.
class OrchardSyncState {
 public:
  struct SpendableNotesBundle {
    SpendableNotesBundle();
    SpendableNotesBundle(const SpendableNotesBundle&) = delete;
    SpendableNotesBundle(SpendableNotesBundle&&);
    SpendableNotesBundle& operator=(const SpendableNotesBundle&) = delete;
    SpendableNotesBundle& operator=(SpendableNotesBundle&&);
    ~SpendableNotesBundle();
    std::vector<OrchardNote> all_notes;
    std::vector<OrchardNote> spendable_notes;
    std::optional<uint32_t> anchor_block_id;
  };

  explicit OrchardSyncState(const base::FilePath& path_to_database);
  virtual ~OrchardSyncState();

  base::expected<OrchardStorage::Result, OrchardStorage::Error> RegisterAccount(
      const mojom::AccountIdPtr& account_id,
      uint64_t account_birthday_block);

  virtual base::expected<std::optional<OrchardStorage::AccountMeta>,
                         OrchardStorage::Error>
  GetAccountMeta(const mojom::AccountIdPtr& account_id);

  virtual base::expected<OrchardStorage::Result, OrchardStorage::Error> Rewind(
      const mojom::AccountIdPtr& account_id,
      uint32_t rewind_block_height,
      const std::string& rewind_block_hash);

  base::expected<std::vector<OrchardNoteSpend>, OrchardStorage::Error>
  GetNullifiers(const mojom::AccountIdPtr& account_id);

  base::expected<OrchardStorage::Result, OrchardStorage::Error>
  ApplyScanResults(const mojom::AccountIdPtr& account_id,
                   // Value is used here to allow moving scanned_blocks which
                   // wraps rust object.
                   OrchardBlockScanner::Result block_scanner_results);

  base::expected<std::optional<uint32_t>, OrchardStorage::Error>
  GetLatestShardIndex(const mojom::AccountIdPtr& account_id);

  virtual base::expected<std::optional<uint32_t>, OrchardStorage::Error>
  GetMinCheckpointId(const mojom::AccountIdPtr& account_id);

  virtual base::expected<std::optional<SpendableNotesBundle>,
                         OrchardStorage::Error>
  GetSpendableNotes(const mojom::AccountIdPtr& account_id,
                    const OrchardAddrRawPart& change_address);

  // Clears sync data related to the account except it's birthday.
  base::expected<OrchardStorage::Result, OrchardStorage::Error>
  ResetAccountSyncState(const mojom::AccountIdPtr& account_id);

  // Drops underlying database.
  void ResetDatabase();

  virtual base::expected<std::vector<OrchardInput>, OrchardStorage::Error>
  CalculateWitnessForCheckpoint(const mojom::AccountIdPtr& account_id,
                                const std::vector<OrchardInput>& notes,
                                uint32_t checkpoint_position);

 private:
  friend class OrchardSyncStateTest;

  // Testing
  void OverrideShardTreeForTesting(
      const mojom::AccountIdPtr& account_id,
      std::unique_ptr<orchard::OrchardShardTree> shard_tree);
  OrchardStorage& orchard_storage();

  orchard::OrchardShardTree& GetOrCreateShardTree(
      const mojom::AccountIdPtr& account_id) LIFETIME_BOUND;

  OrchardStorage storage_;
  std::map<std::string, std::unique_ptr<orchard::OrchardShardTree>>
      shard_trees_;

  SEQUENCE_CHECKER(sequence_checker_);
};

}  // namespace brave_wallet

#endif  // BRAVE_COMPONENTS_BRAVE_WALLET_BROWSER_INTERNAL_ORCHARD_SYNC_STATE_H_
