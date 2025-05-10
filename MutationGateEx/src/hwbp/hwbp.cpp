#include "hwbp.h"

DWORD64 hwbp::getDr7Bits(DWORD64 currDr7, int startingBitPos, int nmberOfBitsToModify, DWORD64 newDr7) {

    ULONG mask = (1UL << nmberOfBitsToModify) - 1UL;
    ULONG newDr7Register = (currDr7 & ~(mask << startingBitPos)) | (newDr7 << startingBitPos);

    return newDr7Register;
}

BOOL hwbp::setHwBp(PVOID pAddress, Drx drx) {

    CONTEXT threadCtx;
    threadCtx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if (!GetThreadContext(GetCurrentThread(), &threadCtx)) {
        return FALSE;
    }

    switch (drx) {
    case Drx::Dr0:
        if (!threadCtx.Dr0) {
            threadCtx.Dr0 = (DWORD64)pAddress;
        }
        break;

    case Drx::Dr1:
        if (!threadCtx.Dr1) {
            threadCtx.Dr1 = (DWORD64)pAddress;
        }
        break;

    case Drx::Dr2:
        if (!threadCtx.Dr2) {
            threadCtx.Dr2 = (DWORD64)pAddress;
        }
        break;

    case Drx::Dr3:
        if (!threadCtx.Dr3) {
            threadCtx.Dr3 = (DWORD64)pAddress;
        }
        break;

    default:
        break;
    }

    threadCtx.Dr7 = getDr7Bits(threadCtx.Dr7, drx * 2, 1, 1);

    if (!SetThreadContext(GetCurrentThread(), &threadCtx)) {
        return FALSE;
    }

    return TRUE;
}

BOOL hwbp::removeHwBp(Drx drx) {

    CONTEXT threadCtx;
    threadCtx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if (!GetThreadContext(GetCurrentThread(), &threadCtx)) {
        return FALSE;
    }

    switch (drx) {
    case Drx::Dr0:
        threadCtx.Dr0 = 0x0;
        break;

    case Drx::Dr1:
        threadCtx.Dr1 = 0x0;
        break;

    case Drx::Dr2:
        threadCtx.Dr2 = 0x0;
        break;

    case Drx::Dr3:
        threadCtx.Dr3 = 0x0;
        break;

    default:
        break;
    }

    threadCtx.Dr7 = getDr7Bits(threadCtx.Dr7, drx * 2, 1, 0);

    if (!SetThreadContext(GetCurrentThread(), &threadCtx)) {
        return FALSE;
    }

    return TRUE;
}


void hwbp::continueExecution(PCONTEXT pCtx) {
    pCtx->EFlags |= (1 << 16);
}

void hwbp::addVeh(PVECTORED_EXCEPTION_HANDLER handler) {
    AddVectoredExceptionHandler(1, handler);
}