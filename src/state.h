// Copyright (c) 2014 The ShadowCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef COIN_STATE_H
#define COIN_STATE_H

#include <string>
#include "sync.h"

enum eNodeType
{
    NT_FULL = 1,
    NT_THIN,
    NT_UNKNOWN // end marker
};

enum eNodeState
{
    NS_STARTUP = 1,
    NS_GET_HEADERS,
    NS_GET_FILTERED_BLOCKS,
    NS_READY,
    
    NS_UNKNOWN // end marker
};

enum eBlockFlags
{
    BLOCK_PROOF_OF_STAKE = (1 << 0), // is proof-of-stake block
    BLOCK_STAKE_ENTROPY  = (1 << 1), // entropy bit for stake modifier
    BLOCK_STAKE_MODIFIER = (1 << 2), // regenerated stake modifier
};


/*  nServices flags
    top 32 bits of CNode::nServices are used to mark services required 
*/

enum
{
    NODE_NETWORK        = (1 << 0),
    THIN_SUPPORT        = (1 << 1),
    THIN_STAKE          = (1 << 2),
    THIN_STEALTH        = (1 << 3),
    SMSG_RELAY          = (1 << 4),
};


const int64_t GENESIS_BLOCK_TIME = 1414644354;

static const int64_t COIN = 100000000;
static const int64_t CENT = 1000000;


extern int nNodeMode;
extern int nNodeState;

extern int nMaxThinPeers;
extern int nBloomFilterElements;


extern int nMinStakeInterval;

extern int nThinStakeDelay;
extern int nThinIndexWindow;
extern int nLastTryThinStake;

static const int nTryStakeMempoolTimeout = 5 * 60; // seconds
static const int nTryStakeMempoolMaxAsk = 16;

extern uint32_t nMaxThinStakeCandidates;

extern uint64_t nLocalServices;
extern uint32_t nLocalRequirements;




extern bool fTestNet;
extern bool fDebug;
extern bool fDebugNet;
extern bool fDebugSmsg;
extern bool fDebugChain;
extern bool fDebugRingSig;
extern bool fNoSmsg;
extern bool fPrintToConsole;
extern bool fPrintToDebugger;
extern bool fRequestShutdown;
extern bool fShutdown;
extern bool fDaemon;
extern bool fServer;
extern bool fCommandLine;
extern std::string strMiscWarning;
extern bool fNoListen;
extern bool fLogTimestamps;
extern bool fReopenDebugLog;
extern bool fThinFullIndex;
extern bool fReindexing;

extern CCriticalSection cs_threadCount;
extern int nThreadCount;

extern unsigned int nStakeSplitAge;
extern int64_t nStakeCombineThreshold;


#endif /* COIN_STATE_H */

