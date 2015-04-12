// Copyright (c) 2014 The ShadowCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#include "state.h"

int nNodeMode = NT_FULL;
int nNodeState = NS_STARTUP;

int nMaxThinPeers = 8;
int nBloomFilterElements = 1536;
int nMinStakeInterval = 30;         // in seconds, min time between successful stakes
int nThinStakeDelay = 48;           // in seconds
int nThinIndexWindow = 4096;        // no. of block headers to keep in memory
int nLastTryThinStake = 0;

uint32_t nMaxThinStakeCandidates = 8;

// -- services provided by local node, initialise to all on
uint64_t nLocalServices     = 0 | NODE_NETWORK | THIN_SUPPORT | THIN_STAKE | THIN_STEALTH | SMSG_RELAY;
uint32_t nLocalRequirements = 0 | NODE_NETWORK;


bool fTestNet = false;
bool fDebug = false;
bool fDebugNet = false;
bool fDebugSmsg = false;
bool fDebugChain = false;
bool fDebugRingSig = false;
bool fNoSmsg = false;
bool fPrintToConsole = false;
bool fPrintToDebugger = false;
bool fRequestShutdown = false;
bool fShutdown = false;
bool fDaemon = false;
bool fServer = false;
bool fCommandLine = false;
std::string strMiscWarning;
bool fNoListen = false;
bool fLogTimestamps = false;
bool fReopenDebugLog = false;
bool fThinFullIndex = false; // when in thin mode don't keep all headers in memory
bool fReindexing = false;

CCriticalSection cs_threadCount;
int nThreadCount = 0;

unsigned int nStakeSplitAge = 1 * 24 * 60 * 60;
int64_t nStakeCombineThreshold = 1000 * COIN;



