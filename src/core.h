// Copyright (c) 2014 The ShadowCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef VTR_CORE_H
#define VTR_CORE_H

#include "util.h"
#include "serialize.h"

#include <stdlib.h> 
#include <stdio.h> 
#include <vector>
#include <inttypes.h>

enum GetMinFee_mode
{
    GMF_BLOCK,
    GMF_RELAY,
    GMF_SEND,
    GMF_ANON,
};

/** An outpoint - a combination of a transaction hash and an index n into its vout */
class COutPoint
{
public:
    uint256 hash;
    unsigned int n;

    COutPoint() { SetNull(); }
    COutPoint(uint256 hashIn, unsigned int nIn) { hash = hashIn; n = nIn; }
    IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
    void SetNull() { hash = 0; n = (unsigned int) -1; }
    bool IsNull() const { return (hash == 0 && n == (unsigned int) -1); }

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash == b.hash && a.n == b.n);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        return strprintf("COutPoint(%s, %u)", hash.ToString().substr(0,10).c_str(), n);
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};



class CKeyImageSpent
{
// stored in txdb, key is keyimage
public:
    CKeyImageSpent() {};
    
    CKeyImageSpent(uint256& txnHash_, uint32_t inputNo_, int64_t nValue_)
    {
        txnHash = txnHash_;
        inputNo = inputNo_;
        nValue  = nValue_;
    };
    
    uint256 txnHash;    // hash of spending transaction
    uint32_t inputNo;   // keyimage is for inputNo of txnHash
    int64_t nValue;     // reporting only
    
    IMPLEMENT_SERIALIZE
    (
        READWRITE(txnHash);
        READWRITE(inputNo);
        READWRITE(nValue);
    )
};

class CAnonOutput
{
// stored in txdb, key is pubkey
public:
    
    CAnonOutput() {};
    
    CAnonOutput(COutPoint& outpoint_, int64_t nValue_, int nBlockHeight_, uint8_t nCompromised_)
    {
        outpoint = outpoint_;
        nValue = nValue_;
        nBlockHeight = nBlockHeight_;
        nCompromised = nCompromised_;
    };
    
    COutPoint outpoint;
    int64_t nValue;         // rather store 2 bytes, digit + power 10 ?
    int nBlockHeight;
    uint8_t nCompromised;   // TODO: mark if output can be identified (spent with ringsig 1)
    IMPLEMENT_SERIALIZE
    (
        READWRITE(outpoint);
        READWRITE(nValue);
        READWRITE(nBlockHeight);
        READWRITE(nCompromised);
    )
};

class CAnonOutputCount
{ // CountAllAnonOutputs
public:
    
    CAnonOutputCount()
    {
        nValue = 0;
        nExists = 0;
        nSpends = 0;
        nOwned = 0;
        nLeastDepth = 0;
    }

    CAnonOutputCount(int64_t nValue_, int nExists_, int nSpends_, int nOwned_, int nLeastDepth_)
    {
        nValue = nValue_;
        nExists = nExists_;
        nSpends = nSpends_;
        nOwned = nOwned_;
        nLeastDepth = nLeastDepth_;
    }
    
    void set(int64_t nValue_, int nExists_, int nSpends_, int nOwned_, int nLeastDepth_)
    {
        nValue = nValue_;
        nExists = nExists_;
        nSpends = nSpends_;
        nOwned = nOwned_;
        nLeastDepth = nLeastDepth_;
    }
    
    void addCoin(int nCoinDepth, int64_t nCoinValue)
    {
        nExists++;
        nValue = nCoinValue;
        if (nCoinDepth < nLeastDepth)
            nLeastDepth = nCoinDepth;
    }
    
    void updateDepth(int nCoinDepth, int64_t nCoinValue)
    {
        nValue = nCoinValue;
        if (nLeastDepth == 0
            || nCoinDepth < nLeastDepth)
            nLeastDepth = nCoinDepth;
    }
    
    void incSpends(int64_t nCoinValue)
    {
        nSpends++;
        nValue = nCoinValue;
    }
    
    void decSpends(int64_t nCoinValue)
    {
        nSpends--;
        nValue = nCoinValue;
    }
    
    void incExists(int64_t nCoinValue)
    {
        nExists++;
        nValue = nCoinValue;
    }
    
    void decExists(int64_t nCoinValue)
    {
        nExists--;
        nValue = nCoinValue;
    }
    
    
    int64_t nValue;
    int nExists;
    int nSpends;
    int nOwned; // todo
    int nLeastDepth;
    
};

#endif  // VTR_CORE_H

