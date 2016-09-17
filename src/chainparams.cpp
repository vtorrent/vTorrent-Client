// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assert.h"

#include "chainparams.h"
#include "main.h"
#ifndef OTP_ENABLED
    #include "util.h"
#else
    #include "util_otp.h"
#endif

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

int64_t CChainParams::GetProofOfWorkReward(int nHeight, int64_t nFees) const
{
    // miner's coin base reward
    int64_t nSubsidy = 0;

    if (nHeight <= 0)
        nSubsidy = 0;
    else    
    if (nHeight <= 20)
        nSubsidy = 30000 * COIN;
	else
	if (nHeight <= nLastPOWBlock)
    nSubsidy = (NetworkID() == CChainParams::TESTNET ? 10000 : 200) * COIN;
            
    if (fDebug && GetBoolArg("-printcreation"))
        LogPrintf("GetProofOfWorkReward() : create=%s nSubsidy=%d\n", FormatMoney(nSubsidy).c_str(), nSubsidy);
    
    return nSubsidy + nFees;
}


int64_t CChainParams::GetProofOfStakeReward(const CBlockIndex* pindexPrev, int64_t nCoinAge, int64_t nFees) const
{
    // miner's coin stake reward based on coin age spent (coin-days)
    int64_t nSubsidy;
    if (IsProtocolV3(pindexPrev->nHeight))
        nSubsidy = (pindexPrev->nMoneySupply / COIN) * COIN_YEAR_REWARD / (365 * 24 * (60 * 60 / 64));
    else
        nSubsidy = nCoinAge * COIN_YEAR_REWARD * 33 / (365 * 33 + 8);

    if (fDebug && GetBoolArg("-printcreation"))
        LogPrintf("GetProofOfStakeReward(): create=%s nCoinAge=%d\n", FormatMoney(nSubsidy).c_str(), nCoinAge);
    
    return nSubsidy + nFees;
}

//
// Main network
//

// Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

// Convert the pnSeeds array into usable address objects.
static void convertSeeds(std::vector<CAddress> &vSeedsOut, const unsigned int *data, unsigned int count, int port)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int k = 0; k < count; ++k)
    {
        struct in_addr ip;
        unsigned int i = data[k], t;
        
        // -- convert to big endian
        t =   (i & 0x000000ff) << 24u
            | (i & 0x0000ff00) << 8u
            | (i & 0x00ff0000) >> 8u
            | (i & 0xff000000) >> 24u;
        
        memcpy(&ip, &t, sizeof(ip));
        
        CAddress addr(CService(ip, port));
        addr.nTime = GetTime()-GetRand(nOneWeek)-nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

class CBaseChainParams : public CChainParams {
public:
    CBaseChainParams() {
        const char* pszTimestamp = "Thu, 30 Oct 2014 13:31:25 GMT";
        std::vector<CTxIn> vin;
        vin.resize(1);
        vin[0].scriptSig = CScript() << 0 << CBigNum(42) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        std::vector<CTxOut> vout;
        vout.resize(1);
        vout[0].SetEmpty();
        CTransaction txNew(1, GENESIS_BLOCK_TIME, vin, vout, 0);
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = GENESIS_BLOCK_TIME;
        
        vSeeds.push_back(CDNSSeedData("vtrseed.mooo.com",  "vtrseed.mooo.com"));
        vSeeds.push_back(CDNSSeedData("vtrseed1.mooo.com",  "vtrseed1.mooo.com"));
        vSeeds.push_back(CDNSSeedData("vtrseed.ignorelist.com",  "vtrseed.ignorelist.com"));
    }
    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual const std::vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    std::vector<CAddress> vFixedSeeds;
};

class CMainParams : public CBaseChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x22;
        pchMessageStart[1] = 0x05;
        pchMessageStart[2] = 0x35;
        pchMessageStart[3] = 0x70;
        
        vAlertPubKey = ParseHex("0418853e2f8c4629f09b49ce09c6ef051928c1dc3ce175b26d2c37300226b5699d5e3ff3e834722ff05fb86c4b69597d58bb8d81d1841dd22911f25ff4d5273d13");
        
        nDefaultPort = 22524;
        nRPCPort = 22523;
        nBIP44ID = 0x80000023;
        
        nLastPOWBlock = 97000;
        
        nFirstPosv2Block = 1200000; // Hardfork v2
        nFirstPosv3Block = 1300000; // Hardfork v3

        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 20); // "standard" scrypt target limit for proof of work, results with 0,000244140625 proof-of-work difficulty
        bnProofOfStakeLimit = CBigNum(~uint256(0) >> 20);
        bnProofOfStakeLimitV2 = CBigNum(~uint256(0) >> 48);
        
        genesis.nBits    = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce   = 223008;
        hashGenesisBlock = genesis.GetHash();
        
        assert(hashGenesisBlock == uint256("0x00000871bd87f95c062614541a3b5533280692b6a88d2dbea84339564dec8cc2"));
        assert(genesis.hashMerkleRoot == uint256("0xe4454b9cb81c1ce04276c70f36d27f7b1872302b2a88af3d602d079dde35a516"));
        
        base58Prefixes[PUBKEY_ADDRESS]      = list_of(75).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SCRIPT_ADDRESS]      = list_of(125).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SECRET_KEY]          = list_of(203).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[STEALTH_ADDRESS]     = list_of(44).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_PUBLIC_KEY]      = list_of(0xEE)(0x80)(0x28)(0x6A).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY]      = list_of(0xEE)(0x80)(0x31)(0xE8).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_KEY_HASH]        = list_of(137).convert_to_container<std::vector<unsigned char> >();         // x
        base58Prefixes[EXT_ACC_HASH]        = list_of(83).convert_to_container<std::vector<unsigned char> >();          // a
        base58Prefixes[EXT_PUBLIC_KEY_BTC]  = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >(); // xprv
        base58Prefixes[EXT_SECRET_KEY_BTC]  = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >(); // xpub

        //convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));
        convertSeeds(vFixedSeeds, pnSeed, ARRAYLEN(pnSeed), nDefaultPort);
    }
    
    virtual Network NetworkID() const { return CChainParams::MAIN; }
};
static CMainParams mainParams;

//
// Testnet
//

class CTestNetParams : public CBaseChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        strDataDir = "testnet";
        
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0x07;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x05;
        pchMessageStart[3] = 0x0b;
        
        vAlertPubKey = ParseHex("04838caf4309cb31991d395f9eda6f3cd6e5928de41aa6432d04a6140a54a6d36dc8dda8ba39f34874f0b917807b425875d5dac3e63e9c9fc22641e17376868cd8");
        
        nDefaultPort = 22525;
        nRPCPort = 22521;
        nBIP44ID = 0x80000001;

        nLastPOWBlock = 300;

        nFirstPosv2Block = 110;
        nFirstPosv3Block = 500;
        
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 16);
        bnProofOfStakeLimit = CBigNum(~uint256(0) >> 20);
        bnProofOfStakeLimitV2 = CBigNum(~uint256(0) >> 16);

        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 0;
        hashGenesisBlock = genesis.GetHash();
//        assert(hashGenesisBlock == uint256("0x00000529c45563ef0037459ebcead0debd6cf1066b91c7dfea5603eb25c963ad"));
        
        base58Prefixes[PUBKEY_ADDRESS]      = list_of(75).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SCRIPT_ADDRESS]      = list_of(125).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SECRET_KEY]          = list_of(203).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[STEALTH_ADDRESS]     = list_of(44).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_PUBLIC_KEY]      = list_of(0xEE)(0x80)(0x28)(0x6A).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY]      = list_of(0xEE)(0x80)(0x31)(0xE8).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_KEY_HASH]        = list_of(137).convert_to_container<std::vector<unsigned char> >();         // x
        base58Prefixes[EXT_ACC_HASH]        = list_of(83).convert_to_container<std::vector<unsigned char> >();          // a
        base58Prefixes[EXT_PUBLIC_KEY_BTC]  = list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >(); // xprv
        base58Prefixes[EXT_SECRET_KEY_BTC]  = list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >(); // xpub
        
        //convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));
        convertSeeds(vFixedSeeds, pnTestnetSeed, ARRAYLEN(pnTestnetSeed), nDefaultPort);
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        strDataDir = "regtest";

        nFirstPosv2Block = -1;
        nFirstPosv3Block = -1;

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        genesis.nTime = 1411111111;
        genesis.nBits  = bnProofOfWorkLimit.GetCompact();
        genesis.nNonce = 0;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 18444;
        
//        assert(hashGenesisBlock == uint256("0xb86b5854a5a77e1eb0a6023b7d6013ebc9840bd87aad7d9e56da458f87015d3f"));

        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.
    }

    virtual bool RequireRPCPassword() const { return false; }
    virtual Network NetworkID() const { return CChainParams::REGTEST; }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

const CChainParams &TestNetParams() {
    return testNetParams;
}

const CChainParams &MainNetParams() {
    return mainParams;
}

void SelectParams(CChainParams::Network network)
{
    switch (network)
    {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        case CChainParams::REGTEST:
            pCurrentParams = &regTestParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    };
};

bool SelectParamsFromCommandLine()
{
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet && fRegTest)
    {
        return false;
    };

    if (fRegTest)
    {
        SelectParams(CChainParams::REGTEST);
    } else
    if (fTestNet)
    {
        SelectParams(CChainParams::TESTNET);
    } else
    {
        SelectParams(CChainParams::MAIN);
    };
    
    return true;
}
