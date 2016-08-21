// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

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
        ( 0,      uint256("0x00000871bd87f95c062614541a3b5533280692b6a88d2dbea84339564dec8cc2") )
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
        ( 300000, uint256("0x3608fe286b091780207169d6cfb775c9574d61653bc30404a1bbceb26a6e6cea") )
        ( 400000, uint256("0x002c8bd959dd6f8066648ce65130a2c1937a0a29d24fbb512b3b957a0b289aec") )
        ( 500000, uint256("0x9cc00af7497ce5a7a4e76cd39f828abbc279a130a988d60278b0c815ed28518e") )
        ( 600000, uint256("0x30dbf673ff8e466d5608f5ab96c6e0affd2167ea49e66efae9ec620a4c31e4e0") )
        ( 700000, uint256("0x58d8a9596d9299997631b0d738eec3f7ba127c5ca084932ebde6b440eaae94cf") )
        ( 800000, uint256("0x4af6a98d7e3652cfed524c46e3ec24ebe0549cc7dabb1a195c88d40f7422dec2") )
        ( 850000, uint256("0x05aaf025046da05ec42de3a8117280e9dcb68be04e6253303bf637e7b7f9601e") )
        ( 900000, uint256("0x5b6cfbf8c6dbeee760a1dea1b1f0d8be3b4d3e2bf2588c1f96b6636aeb26fcf9") )
        ( 950000, uint256("0x7287c82e068b9a1bd4f4161ee06e232b8b1ac8b6c8b2c064bfa885d6f00dc78d") )
        ( 1000000, uint256("0x4709116102f435cb21fc5ae7b617aec4a6af6052278de6930250bed673974811") )
    ;

    // TestNet has no checkpoints
    MapCheckpoints mapCheckpointsTestnet;

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

        if (checkpoints.empty())
            return 0;
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
        }
        return NULL;
    }

    CBlockThinIndex* GetLastCheckpoint(const std::map<uint256, CBlockThinIndex*>& mapBlockThinIndex)
    {
        MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockThinIndex*>::const_iterator t = mapBlockThinIndex.find(hash);
            if (t != mapBlockThinIndex.end())
                return t->second;
        }
        return NULL;
    }


    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Automatically select a suitable sync-checkpoint - Thin mode
    const CBlockThinIndex* AutoSelectSyncThinCheckpoint()
    {
        const CBlockThinIndex *pindex = pindexBestHeader;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        if(nNodeMode == NT_FULL)
        {
            const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

            if (nHeight <= pindexSync->nHeight)
                return false;
        }
        else {
            const CBlockThinIndex *pindexSync = AutoSelectSyncThinCheckpoint();

            if (nHeight <= pindexSync->nHeight)
                return false;
        }
        return true;
    }
}
