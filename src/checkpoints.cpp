// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 10;

namespace Checkpoints
{
    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0,      hashGenesisBlock )
        ( 5000,   uint256("0x000000000013e9f79773552c432b3bbe5bc7ede7ca4ed4b8279868f5831bec92") )
        ( 10000,  uint256("0x37c210fa5385f30ee2c78e0fe2d9e4d38157569145f4454155196e5f5703af9b") )
        ( 15000,  uint256("0x8760db5b7a0fbef3d8266b6eb459d6802c0186410e0771505532b75bdfd4b509") )
        ( 20000,  uint256("0x0000000002763fa35a540c402b695586537d515197b961bd871a538c0d986b2a") )
        ( 25000,  uint256("0x0000000005dd7fbae0a02cd99507d7f91bb7f0d808799d63860736454356ce4a") )
        ( 30000,  uint256("0x2330d3cb6e454a6a7e16db661266194d53dc3026a709a4144eeacc34a375b7df") )
        ( 40000,  uint256("0x817a56945e37b81a96335d1f6097dbdea18831b4966612d096d1ef1c3191eb30") )
        ( 50000,  uint256("0xc47fd098bcec6d77fece64400c360034ad08efa00d898eec1d433093a71426e0") )
        ( 60000,  uint256("0x5e8a21920e81563c77447f0f784331375a73bc0136de12cf9da81bf94e4c481c") )
        ( 70000,  uint256("0x00000000029a1a5a55c5e1ef75a4755a8d212faffc05fc8b3bafae20150a279d") )
        ( 80000,  uint256("0x0000000000ceeb75e77dc07a5c0693c347aa44f321822444fc2824662915e572") )
        ( 90000,  uint256("0x433c9dbb4a1c2821bde92634bfffd65ce051ab9290c84fb00ec5ff6e6f349868") )
        ( 100000, uint256("0x4fdfcfeba7740aabb9154f1e43cf8be02d28642b44f18ed41f6c9afba5be37f1") )
        ( 110000, uint256("0xee27fa93f9eecd97e1cd01f7024111318ccecdc6010c20ff95f60a1d6f3929e8") )
        ( 120000, uint256("0xcc4bb599f4c4276d70aff4b77b38faacf5e8848c448d6631239ff03bebe30b86") )
        ( 130000, uint256("0x92155eda5eb69957a718b1740f2328fb87a133109992c22d139e63b514a17df7") )
        ( 140000, uint256("0xbbe85110d7eda047ee611c3223485a04c804517b788a1ead28563a8c1300341b") )
        ( 150000, uint256("0xfb8afc32f880852023b4d77b3b58dba1724769b7e59ff2122eb1500fd6647fd1") )
        ( 160000, uint256("0xef43cbe47c12d2ce7bd13ee25074ccfc5a7a8597a2d71295846ebc28a45013ee") )
        ( 170000, uint256("0x2e73f79745bee83d843274d55141e836b5cbbcb05bd1fbd4e2eae071b41b6b0e") )
        ( 180000, uint256("0x566b37cbf220f038e26871c97750817ae62d4f0e2a0cd7648ad94ab2a960bda4") )
        ( 190000, uint256("0xfd1d560081748e557f533b0943c33fc2376bfb6f4132b019bb5b3c07fdbf5cb4") )
        ( 200000, uint256("0x7a928126e9cc1ad70e8a748e5e37394ef29d88e9d5c8436ae68b81301a707c4b") )
        ( 210000, uint256("0x2538dc72f98c1a7dced065885ac34f49c867760ea28557c351f57fa442db85a4") )
        ( 220000, uint256("0xeedbf843fe8cb0b70859a5e71165bfc5ee8d3efcf55c6543ba51fab701ab88ce") )
    ;

    // TestNet has no checkpoints
    MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        ( 0, hashGenesisBlockTestNet )
    ;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end())
            return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        };

        return NULL;
    }

    // ppcoin: synchronized checkpoint (centrally broadcasted)
    uint256 hashSyncCheckpoint = 0;
    uint256 hashPendingCheckpoint = 0;
    CSyncCheckpoint checkpointMessage;
    CSyncCheckpoint checkpointMessagePending;
    uint256 hashInvalidCheckpoint = 0;
    CCriticalSection cs_hashSyncCheckpoint;

    // ppcoin: get last synchronized checkpoint
    CBlockIndex* GetLastSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        if (!mapBlockIndex.count(hashSyncCheckpoint))
            printf("GetSyncCheckpoint: block index missing for current sync-checkpoint %s", hashSyncCheckpoint.ToString().c_str());
        else
            return mapBlockIndex[hashSyncCheckpoint];
        return NULL;
    }

    CBlockThinIndex* GetLastSyncCheckpointHeader()
    {
        LOCK(cs_hashSyncCheckpoint);
        if (!mapBlockThinIndex.count(hashSyncCheckpoint))
            error("GetLastSyncCheckpointHeader: block index missing for current sync-checkpoint %s", hashSyncCheckpoint.ToString().c_str());
        else
            return mapBlockThinIndex[hashSyncCheckpoint];
        return NULL;
    }

    // ppcoin: only descendant of current sync-checkpoint is allowed
    bool ValidateSyncCheckpoint(uint256 hashCheckpoint)
    {
        if (!mapBlockIndex.count(hashSyncCheckpoint))
            return error("ValidateSyncCheckpoint: block index missing for current sync-checkpoint %s", hashSyncCheckpoint.ToString().c_str());
        if (!mapBlockIndex.count(hashCheckpoint))
            return error("ValidateSyncCheckpoint: block index missing for received sync-checkpoint %s", hashCheckpoint.ToString().c_str());

        CBlockIndex* pindexSyncCheckpoint = mapBlockIndex[hashSyncCheckpoint];
        CBlockIndex* pindexCheckpointRecv = mapBlockIndex[hashCheckpoint];

        if (pindexCheckpointRecv->nHeight <= pindexSyncCheckpoint->nHeight)
        {
            // Received an older checkpoint, trace back from current checkpoint
            // to the same height of the received checkpoint to verify
            // that current checkpoint should be a descendant block
            CBlockIndex* pindex = pindexSyncCheckpoint;
            while (pindex->nHeight > pindexCheckpointRecv->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("ValidateSyncCheckpoint: pprev null - block index structure failure");
            if (pindex->GetBlockHash() != hashCheckpoint)
            {
                hashInvalidCheckpoint = hashCheckpoint;
                return error("ValidateSyncCheckpoint: new sync-checkpoint %s is conflicting with current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
            }
            return false; // ignore older checkpoint
        }

        // Received checkpoint should be a descendant block of the current
        // checkpoint. Trace back to the same height of current checkpoint
        // to verify.
        CBlockIndex* pindex = pindexCheckpointRecv;
        while (pindex->nHeight > pindexSyncCheckpoint->nHeight)
            if (!(pindex = pindex->pprev))
                return error("ValidateSyncCheckpoint: pprev2 null - block index structure failure");
        if (pindex->GetBlockHash() != hashSyncCheckpoint)
        {
            hashInvalidCheckpoint = hashCheckpoint;
            return error("ValidateSyncCheckpoint: new sync-checkpoint %s is not a descendant of current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
        };

        return true;
    }

    bool ValidateSyncCheckpointHeaders(uint256 hashCheckpoint)
    {

        if (!mapBlockThinIndex.count(hashCheckpoint))
            return error("ValidateSyncCheckpointHeaders: block index missing for received sync-checkpoint %s", hashCheckpoint.ToString().c_str());

        CBlockThinIndex* pindexCheckpointRecv = mapBlockThinIndex[hashCheckpoint];


        if (!mapBlockThinIndex.count(hashSyncCheckpoint))
        {
            if (fThinFullIndex)
                return error("ValidateSyncCheckpointHeaders: block index missing for current sync-checkpoint %s", hashSyncCheckpoint.ToString().c_str());

            //if hashSyncCheckpoint
            //printf("[rem] ValidateSyncCheckpointHeaders ReadBlockThinIndex\n");
            CTxDB txdb("r");
            CDiskBlockThinIndex diskindex;
            if (!txdb.ReadBlockThinIndex(hashCheckpoint, diskindex)
                || diskindex.hashNext == 0)
                return error("ValidateSyncCheckpointHeaders() : block not in db %s", hashCheckpoint.ToString().c_str());


            // TODO: add checks
            // so long as it's in the db with hashNext it should be in the main chain

            return true;
        };


        CBlockThinIndex* pindexSyncCheckpoint = mapBlockThinIndex[hashSyncCheckpoint];


        if (pindexCheckpointRecv->nHeight <= pindexSyncCheckpoint->nHeight)
        {
            // Received an older checkpoint, trace back from current checkpoint
            // to the same height of the received checkpoint to verify
            // that current checkpoint should be a descendant block
            CBlockThinIndex* pindex = pindexSyncCheckpoint;
            while (pindex->nHeight > pindexCheckpointRecv->nHeight)
            {
                if (!(pindex = pindex->pprev))
                    return error("ValidateSyncCheckpointHeaders: pprev null - block index structure failure");
            };

            if (pindex->GetBlockHash() != hashCheckpoint)
            {
                hashInvalidCheckpoint = hashCheckpoint;
                return error("ValidateSyncCheckpointHeaders: new sync-checkpoint %s is conflicting with current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
            };

            return false; // ignore older checkpoint
        };

        // Received checkpoint should be a descendant block of the current
        // checkpoint. Trace back to the same height of current checkpoint
        // to verify.
        CBlockThinIndex* pindex = pindexCheckpointRecv;
        while (pindex->nHeight > pindexSyncCheckpoint->nHeight)
        {
            if (!(pindex = pindex->pprev))
                return error("ValidateSyncCheckpointHeaders: pprev2 null - block index structure failure");
        };

        if (pindex->GetBlockHash() != hashSyncCheckpoint)
        {
            hashInvalidCheckpoint = hashCheckpoint;
            return error("ValidateSyncCheckpointHeaders: new sync-checkpoint %s is not a descendant of current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
        };
        return true;
    }

    bool WriteSyncCheckpoint(const uint256& hashCheckpoint)
    {
        CTxDB txdb;
        txdb.TxnBegin();
        if (!txdb.WriteSyncCheckpoint(hashCheckpoint))
        {
            txdb.TxnAbort();
            return error("WriteSyncCheckpoint(): failed to write to db sync checkpoint %s", hashCheckpoint.ToString().c_str());
        }
        if (!txdb.TxnCommit())
            return error("WriteSyncCheckpoint(): failed to commit to db sync checkpoint %s", hashCheckpoint.ToString().c_str());

        Checkpoints::hashSyncCheckpoint = hashCheckpoint;
        return true;
    }

    bool AcceptPendingSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint != 0 && mapBlockIndex.count(hashPendingCheckpoint))
        {
            if (!ValidateSyncCheckpoint(hashPendingCheckpoint))
            {
                hashPendingCheckpoint = 0;
                checkpointMessagePending.SetNull();
                return false;
            }

            CTxDB txdb;
            CBlockIndex* pindexCheckpoint = mapBlockIndex[hashPendingCheckpoint];
            if (!pindexCheckpoint->IsInMainChain())
            {
                CBlock block;
                if (!block.ReadFromDisk(pindexCheckpoint))
                    return error("AcceptPendingSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());

                if (!block.SetBestChain(txdb, pindexCheckpoint))
                {
                    hashInvalidCheckpoint = hashPendingCheckpoint;
                    return error("AcceptPendingSyncCheckpoint: SetBestChain failed for sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
                };
            };

            if (!WriteSyncCheckpoint(hashPendingCheckpoint))
                return error("AcceptPendingSyncCheckpoint(): failed to write sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());

            hashPendingCheckpoint = 0;
            checkpointMessage = checkpointMessagePending;
            checkpointMessagePending.SetNull();
            printf("AcceptPendingSyncCheckpoint : sync-checkpoint at %s\n", hashSyncCheckpoint.ToString().c_str());

            // relay the checkpoint
            if (!checkpointMessage.IsNull())
            {
                BOOST_FOREACH(CNode* pnode, vNodes)
                    checkpointMessage.RelayTo(pnode);
            }
            return true;
        }
        return false;
    }

    // Automatically select a suitable sync-checkpoint
    uint256 AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && (pindex->GetBlockTime() + nCheckpointSpan * nTargetSpacing > pindexBest->GetBlockTime() || pindex->nHeight + nCheckpointSpan > pindexBest->nHeight))
            pindex = pindex->pprev;
        return pindex->GetBlockHash();
    }

    // Check against synchronized checkpoint
    bool CheckSync(const uint256& hashBlock, const CBlockIndex* pindexPrev)
    {
        if (fTestNet) return true; // Testnet has no checkpoints
        int nHeight = pindexPrev->nHeight + 1;

        LOCK(cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];

        if (nHeight > pindexSync->nHeight)
        {
            // trace back to same height as sync-checkpoint
            const CBlockIndex* pindex = pindexPrev;
            while (pindex->nHeight > pindexSync->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("CheckSync: pprev null - block index structure failure");
            if (pindex->nHeight < pindexSync->nHeight || pindex->GetBlockHash() != hashSyncCheckpoint)
                return false; // only descendant of sync-checkpoint can pass check
        };

        if (nHeight == pindexSync->nHeight && hashBlock != hashSyncCheckpoint)
            return false; // same height with sync-checkpoint
        if (nHeight < pindexSync->nHeight && !mapBlockIndex.count(hashBlock))
            return false; // lower height than sync-checkpoint
        return true;
    }

    bool WantedByPendingSyncCheckpoint(uint256 hashBlock)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint == 0)
            return false;
        if (hashBlock == hashPendingCheckpoint)
            return true;
        if (mapOrphanBlocks.count(hashPendingCheckpoint)
            && hashBlock == WantedByOrphan(mapOrphanBlocks[hashPendingCheckpoint]))
            return true;
        return false;
    }

    bool WantedByPendingSyncCheckpointHeader(uint256 hashBlock)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint == 0)
            return false;
        if (hashBlock == hashPendingCheckpoint)
            return true;
        if (mapOrphanBlockThins.count(hashPendingCheckpoint)
            && hashBlock == WantedByOrphanHeader(mapOrphanBlockThins[hashPendingCheckpoint]))
            return true;
        return false;
    }

    // ppcoin: reset synchronized checkpoint to last hardened checkpoint
    bool ResetSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        const uint256& hash = mapCheckpoints.rbegin()->second;
        if (mapBlockIndex.count(hash) && !mapBlockIndex[hash]->IsInMainChain())
        {
            // checkpoint block accepted but not yet in main chain
            printf("ResetSyncCheckpoint: SetBestChain to hardened checkpoint %s\n", hash.ToString().c_str());
            CTxDB txdb;
            CBlock block;
            if (!block.ReadFromDisk(mapBlockIndex[hash]))
                return error("ResetSyncCheckpoint: ReadFromDisk failed for hardened checkpoint %s", hash.ToString().c_str());
            if (!block.SetBestChain(txdb, mapBlockIndex[hash]))
            {
                return error("ResetSyncCheckpoint: SetBestChain failed for hardened checkpoint %s", hash.ToString().c_str());
            };
        } else
        if (!mapBlockIndex.count(hash))
        {
            // checkpoint block not yet accepted
            hashPendingCheckpoint = hash;
            checkpointMessagePending.SetNull();
            printf("ResetSyncCheckpoint: pending for sync-checkpoint %s\n", hashPendingCheckpoint.ToString().c_str());
        };

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, mapCheckpoints)
        {
            const uint256& hash = i.second;
            if (mapBlockIndex.count(hash) && mapBlockIndex[hash]->IsInMainChain())
            {
                if (!WriteSyncCheckpoint(hash))
                    return error("ResetSyncCheckpoint: failed to write sync checkpoint %s", hash.ToString().c_str());
                printf("ResetSyncCheckpoint: sync-checkpoint reset to %s\n", hashSyncCheckpoint.ToString().c_str());
                return true;
            };
        };

        return false;
    }

    bool ResetSyncCheckpointThin()
    {
        LOCK(cs_hashSyncCheckpoint);
        const uint256& hash = mapCheckpoints.rbegin()->second;
        if (mapBlockThinIndex.count(hash) && !mapBlockThinIndex[hash]->IsInMainChain())
        {
            // checkpoint block accepted but not yet in main chain
            printf("ResetSyncCheckpointThin: SetBestChain to hardened checkpoint %s\n", hash.ToString().c_str());

            CBlockThin block = mapBlockThinIndex[hash]->GetBlockThin();
            CTxDB txdb;
            if (!block.SetBestThinChain(txdb, mapBlockThinIndex[hash]))
            {
                return error("ResetSyncCheckpointThin: SetBestChain failed for sync checkpoint %s", hash.ToString().c_str());
            };

        } else
        if (!mapBlockThinIndex.count(hash))
        {
            // checkpoint block not yet accepted
            hashPendingCheckpoint = hash;
            checkpointMessagePending.SetNull();
            printf("ResetSyncCheckpointThin: pending for sync-checkpoint %s\n", hashPendingCheckpoint.ToString().c_str());
        };

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, mapCheckpoints)
        {
            const uint256& hash = i.second;
            if (mapBlockThinIndex.count(hash) && mapBlockThinIndex[hash]->IsInMainChain())
            {
                if (!WriteSyncCheckpoint(hash))
                    return error("ResetSyncCheckpoint: failed to write sync checkpoint %s", hash.ToString().c_str());
                printf("ResetSyncCheckpoint: sync-checkpoint reset to %s\n", hashSyncCheckpoint.ToString().c_str());
                return true;
            };
        };

        return false;
    }

    void AskForPendingSyncCheckpoint(CNode* pfrom)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (pfrom && hashPendingCheckpoint != 0 && (!mapBlockIndex.count(hashPendingCheckpoint)) && (!mapOrphanBlocks.count(hashPendingCheckpoint)))
            pfrom->AskFor(CInv(MSG_BLOCK, hashPendingCheckpoint));
    }

    bool SetCheckpointPrivKey(std::string strPrivKey)
    {
        if (fDebug)
            printf("SetCheckpointPrivKey()\n");

        // Test signing a sync-checkpoint with genesis block
        CSyncCheckpoint checkpoint;
        checkpoint.hashCheckpoint = !fTestNet ? hashGenesisBlock : hashGenesisBlockTestNet;
        CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
        sMsg << (CUnsignedSyncCheckpoint)checkpoint;
        checkpoint.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());

        std::vector<unsigned char> vchPrivKey = ParseHex(strPrivKey);
        CKey key;
        key.SetPrivKey(CPrivKey(vchPrivKey.begin(), vchPrivKey.end()),false); // if key is not correct openssl may crash
        if (!key.Sign(Hash(checkpoint.vchMsg.begin(), checkpoint.vchMsg.end()), checkpoint.vchSig))
            return false;

        // Test signing successful, proceed
        CSyncCheckpoint::strMasterPrivKey = strPrivKey;
        return true;
    }

    bool SendSyncCheckpoint(uint256 hashCheckpoint)
    {
        if (fDebug)
            printf("SendSyncCheckpoint()\n");

        CSyncCheckpoint checkpoint;
        checkpoint.hashCheckpoint = hashCheckpoint;
        CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
        sMsg << (CUnsignedSyncCheckpoint)checkpoint;
        checkpoint.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());

        if (CSyncCheckpoint::strMasterPrivKey.empty())
            return error("SendSyncCheckpoint: Checkpoint master key unavailable.");

        std::vector<unsigned char> vchPrivKey = ParseHex(CSyncCheckpoint::strMasterPrivKey);
        CKey key;
        key.SetPrivKey(CPrivKey(vchPrivKey.begin(), vchPrivKey.end()),false); // if key is not correct openssl may crash
        if (!key.Sign(Hash(checkpoint.vchMsg.begin(), checkpoint.vchMsg.end()), checkpoint.vchSig))
            return error("SendSyncCheckpoint: Unable to sign checkpoint, check private key?");

        if (!checkpoint.ProcessSyncCheckpoint(NULL))
        {
            printf("WARNING: SendSyncCheckpoint: Failed to process checkpoint.\n");
            return false;
        };

        // Relay checkpoint
        {
            LOCK(cs_vNodes);
            BOOST_FOREACH(CNode* pnode, vNodes)
                checkpoint.RelayTo(pnode);
        }
        return true;
    }

    // Is the sync-checkpoint outside maturity window?
    bool IsMatureSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];
        return (nBestHeight >= pindexSync->nHeight + nCoinbaseMaturity ||
                pindexSync->GetBlockTime() + nStakeMinAge < GetAdjustedTime());
    }
}

// ppcoin: sync-checkpoint master key                 
const std::string CSyncCheckpoint::strMasterPubKey = "0418853e2f8c4629f09b49ce09c6ef051928c1dc3ce175b26d2c37300226b5699d5e3ff3e834722ff05fb86c4b69597d58bb8d81d1841dd22911f25ff4d5273d13";

std::string CSyncCheckpoint::strMasterPrivKey = "";

// ppcoin: verify signature of sync-checkpoint message
bool CSyncCheckpoint::CheckSignature()
{
    CPubKey key(ParseHex(CSyncCheckpoint::strMasterPubKey));
    if (!key.Verify(Hash(vchMsg.begin(), vchMsg.end()), vchSig))
        return error("CSyncCheckpoint::CheckSignature() : verify signature failed");

    // Now unserialize the data
    CDataStream sMsg(vchMsg, SER_NETWORK, PROTOCOL_VERSION);
    sMsg >> *(CUnsignedSyncCheckpoint*)this;
    return true;
}

// ppcoin: process synchronized checkpoint
bool CSyncCheckpoint::ProcessSyncCheckpoint(CNode* pfrom)
{
    if (!CheckSignature())
        return false;

    LOCK(Checkpoints::cs_hashSyncCheckpoint);
    if (!mapBlockIndex.count(hashCheckpoint))
    {
        // We haven't received the checkpoint chain, keep the checkpoint as pending
        Checkpoints::hashPendingCheckpoint = hashCheckpoint;
        Checkpoints::checkpointMessagePending = *this;
        printf("ProcessSyncCheckpoint: pending for sync-checkpoint %s\n", hashCheckpoint.ToString().c_str());
        // Ask this guy to fill in what we're missing
        if (pfrom)
        {
            pfrom->PushGetBlocks(pindexBest, hashCheckpoint);

            // ask directly as well in case rejected earlier by duplicate
            // proof-of-stake because getblocks may not get it this time
            pfrom->AskFor(CInv(MSG_BLOCK, mapOrphanBlocks.count(hashCheckpoint) ? WantedByOrphan(mapOrphanBlocks[hashCheckpoint]) : hashCheckpoint));
        };
        return false;
    };

    if (!Checkpoints::ValidateSyncCheckpoint(hashCheckpoint))
        return false;

    CTxDB txdb;
    CBlockIndex* pindexCheckpoint = mapBlockIndex[hashCheckpoint];
    if (!pindexCheckpoint->IsInMainChain())
    {
        // checkpoint chain received but not yet main chain
        CBlock block;
        if (!block.ReadFromDisk(pindexCheckpoint))
            return error("ProcessSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());

        if (!block.SetBestChain(txdb, pindexCheckpoint))
        {
            Checkpoints::hashInvalidCheckpoint = hashCheckpoint;
            return error("ProcessSyncCheckpoint: SetBestChain failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());
        };
    };

    if (!Checkpoints::WriteSyncCheckpoint(hashCheckpoint))
        return error("ProcessSyncCheckpoint(): failed to write sync checkpoint %s", hashCheckpoint.ToString().c_str());
    Checkpoints::checkpointMessage = *this;
    Checkpoints::hashPendingCheckpoint = 0;
    Checkpoints::checkpointMessagePending.SetNull();
    printf("ProcessSyncCheckpoint: sync-checkpoint at %s\n", hashCheckpoint.ToString().c_str());
    return true;
}

bool CSyncCheckpoint::ProcessSyncCheckpointHeaders(CNode* pfrom)
{
    if (!CheckSignature())
        return false;

    LOCK(Checkpoints::cs_hashSyncCheckpoint);
    if (!mapBlockThinIndex.count(hashCheckpoint))
    {
        // We haven't received the checkpoint chain, keep the checkpoint as pending
        Checkpoints::hashPendingCheckpoint = hashCheckpoint;
        Checkpoints::checkpointMessagePending = *this;
        printf("ProcessSyncCheckpointHeaders: pending for sync-checkpoint %s\n", hashCheckpoint.ToString().c_str());

        // Ask this guy to fill in what we're missing
        if (pfrom)
        {
            pfrom->PushGetBlocks(pindexBestHeader, hashCheckpoint);

            // ask directly as well in case rejected earlier by duplicate
            // proof-of-stake because getblocks may not get it this time

            if (nNodeMode != NT_FULL)
                pfrom->AskFor(CInv(MSG_FILTERED_BLOCK, mapOrphanBlockThins.count(hashCheckpoint) ? WantedByOrphanHeader(mapOrphanBlockThins[hashCheckpoint]) : hashCheckpoint));
            else
                pfrom->AskFor(CInv(MSG_BLOCK, mapOrphanBlockThins.count(hashCheckpoint) ? WantedByOrphanHeader(mapOrphanBlockThins[hashCheckpoint]) : hashCheckpoint));
        };

        return false;
    };

    if (!Checkpoints::ValidateSyncCheckpointHeaders(hashCheckpoint))
        return false;

    CTxDB txdb;
    CBlockThinIndex* pindexCheckpoint = mapBlockThinIndex[hashCheckpoint];
    if (!pindexCheckpoint->IsInMainChain())
    {
        // checkpoint chain received but not yet main chain
        CBlockThin block = pindexCheckpoint->GetBlockThin();
        //if (!block.ReadFromDisk(pindexCheckpoint))
        //    return error("ProcessSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());

        if (!block.SetBestThinChain(txdb, pindexCheckpoint))
        {
            Checkpoints::hashInvalidCheckpoint = hashCheckpoint;
            return error("ProcessSyncCheckpointHeaders: SetBestChain failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());
        };
    };

    if (!Checkpoints::WriteSyncCheckpoint(hashCheckpoint))
        return error("ProcessSyncCheckpointHeaders(): failed to write sync checkpoint %s", hashCheckpoint.ToString().c_str());

    Checkpoints::checkpointMessage = *this;
    Checkpoints::hashPendingCheckpoint = 0;
    Checkpoints::checkpointMessagePending.SetNull();
    printf("ProcessSyncCheckpointHeaders: sync-checkpoint at %s\n", hashCheckpoint.ToString().c_str());
    return true;
}
