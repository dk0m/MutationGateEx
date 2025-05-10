#pragma once

#include<windows.h>

enum Drx {
    Dr0,
    Dr1,
    Dr2,
    Dr3,
};

namespace hwbp {
    DWORD64 getDr7Bits(DWORD64 currDr7, int startingBitPos, int nmberOfBitsToModify, DWORD64 newDr7);
    BOOL setHwBp(PVOID pAddress, Drx drx);
    BOOL removeHwBp(Drx drx);
    void continueExecution(PCONTEXT pCtx);

    void addVeh(PVECTORED_EXCEPTION_HANDLER handler);

    template <typename T>
    void setRax(PCONTEXT pCtx, T value) {
        pCtx->Rax = (DWORD64)value;
    }

}