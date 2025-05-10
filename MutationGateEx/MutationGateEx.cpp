#include<iostream>

#include "./src/hwbp/hwbp.h"
#include "./src/ssn/ssn.h"

typedef NTSTATUS(NTAPI* fnNtQueryInformationProcess)(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

LONG exceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {

	auto exceptionCode = pExceptionInfo->ExceptionRecord->ExceptionCode;
	auto exceptionAddress = (DWORD64)pExceptionInfo->ExceptionRecord->ExceptionAddress;
	auto pCtx = pExceptionInfo->ContextRecord;

	if (exceptionCode == EXCEPTION_SINGLE_STEP) {

		Drx drx = (Drx)-1;

		if (exceptionAddress == pCtx->Dr0) {
			drx = Drx::Dr0;
		}
		else if (exceptionAddress == pCtx->Dr1) {
			drx = Drx::Dr1;
		}
		else if (exceptionAddress == pCtx->Dr2) {
			drx = Drx::Dr2;
		}
		else if (exceptionAddress == pCtx->Dr3) {
			drx = Drx::Dr3;
		}

		auto state = ssn::getState();

		hwbp::setRax<uint32_t>(pCtx, state.ssn);

		hwbp::removeHwBp(drx);
		hwbp::continueExecution(pCtx);

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else {
		return EXCEPTION_CONTINUE_SEARCH;
	}

}

int main() {

	ssn::populateEntries(PopulationMethod::Rtf);

	auto targetFnHash = hash::ror13Hash("ZwDrawText");
	auto targetFn = ssn::getNtFunctionByHash(targetFnHash);

	auto fnHash = hash::ror13Hash("ZwQueryInformationProcess");
	auto optEntry = ssn::findEntryByHash(fnHash);

	if (!optEntry.has_value()) {
		printf("[-] Couldn't Find Entry.");
		return -1;
	}

	auto entry = optEntry.value();
	ssn::setState(entry);

	printf("[+] Found Entry, SSN: %x\n", entry.ssn);

	hwbp::addVeh(exceptionHandler);
	hwbp::setHwBp((PVOID)((ULONG_PTR)targetFn + 0x8), Drx::Dr0);

	fnNtQueryInformationProcess NtQueryInformationProcess = (fnNtQueryInformationProcess)targetFn;

	PROCESS_BASIC_INFORMATION pbi{ 0 };
	NTSTATUS status = NtQueryInformationProcess(
		GetCurrentProcess(),
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(pbi),
		NULL
	);

	if (NT_SUCCESS(status)) {
		printf("[+] Call Successful, Status: 0x%x\n", status);
		printf("[*] Current Process PEB Address: 0x%p\n", pbi.PebBaseAddress);
	}
	else {
		printf("[-] Call Failed, Status: 0x%x\n", status);
	}

	system("pause");
}