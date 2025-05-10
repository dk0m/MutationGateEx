#include "ssn.h"

uint32_t hash::ror13(uint32_t functionHash, int bits) {
	return ((functionHash >> bits) | (functionHash << (32 - bits))) & 0xFFFFFFFF;
}

uint32_t hash::ror13Hash(LPCSTR functionName) {
	uint32_t functionHash = 0;
	for (int i = 0; functionName[i] != '\0'; i++) {
		uint32_t c = (uint32_t)functionName[i];
		functionHash = ror13(functionHash, 13);
		functionHash = functionHash + c;
	}
	return functionHash;
}

SSNEntry ssn::getState() {
	return gState;
}
void ssn::setState(SSNEntry& entry) {
	gState = entry;
}

void ssn::rtfPopulateEntries() {
	auto baseNtdll = ntdll.OptionalHeader.ImageBase;

	auto expDir = ntdll.ExportDirectory;
	auto rtf = ntdll.RunTimeEntryTable;

	PDWORD addrOfNameRvas = (PDWORD)(baseNtdll + expDir->AddressOfNames);
	PWORD addrOfOrds = (PWORD)(baseNtdll + expDir->AddressOfNameOrdinals);
	PDWORD addrOfFnRvas = (PDWORD)(baseNtdll + expDir->AddressOfFunctions);

	DWORD index = 0;
	DWORD ssn = 0;

	while (rtf[index].BeginAddress) {

		for (size_t i = 0; i < expDir->NumberOfFunctions; i++)
		{
			LPCSTR fnName = (LPCSTR)(baseNtdll + addrOfNameRvas[i]);

			if (strncmp(fnName, "Zw", 2))
				continue;

			WORD fnOrd = addrOfOrds[i];
			DWORD fnRva = addrOfFnRvas[fnOrd];


			if (fnRva == rtf[index].BeginAddress) {

				auto entry = SSNEntry {
					hash::ror13Hash(fnName),
					ssn
				};

				entries.push_back(entry);
				ssn++;
			}

		}

		index++;
	}
}

void ssn::cfgPopulateEntries() {
	auto baseNtdll = ntdll.OptionalHeader.ImageBase;

	auto expDir = ntdll.ExportDirectory;
	auto cfg = ntdll.GuardCffTable;

	PDWORD addrOfNameRvas = (PDWORD)(baseNtdll + expDir->AddressOfNames);
	PWORD addrOfOrds = (PWORD)(baseNtdll + expDir->AddressOfNameOrdinals);
	PDWORD addrOfFnRvas = (PDWORD)(baseNtdll + expDir->AddressOfFunctions);

	DWORD index = 0;
	DWORD ssn = 0;

	while (cfg[index].Rva) {

		for (size_t i = 0; i < expDir->NumberOfFunctions; i++)
		{
			LPCSTR fnName = (LPCSTR)(baseNtdll + addrOfNameRvas[i]);

			if (strncmp(fnName, "Zw", 2))
				continue;

			WORD fnOrd = addrOfOrds[i];
			DWORD fnRva = addrOfFnRvas[fnOrd];


			if (fnRva == cfg[index].Rva) {

				auto entry = SSNEntry{
					hash::ror13Hash(fnName),
					ssn
				};

				entries.push_back(entry);
				ssn++;
			}

		}

		index++;
	}
}

void ssn::populateEntries(PopulationMethod method = PopulationMethod::Rtf) {

	switch (method) {

	case PopulationMethod::Rtf:
		rtfPopulateEntries();
		break;

	case PopulationMethod::Cfg:
		cfgPopulateEntries();
		break;
	}

}

std::optional<SSNEntry> ssn::findEntryByHash(uint32_t hash) {
	for (auto& entry : entries) {

		if (entry.hash == hash) {
			return std::make_optional<SSNEntry>(entry);
		}

	}

	return std::nullopt;
}


PVOID ssn::getNtFunctionByHash(uint32_t hash) {
	auto baseNtdll = ntdll.OptionalHeader.ImageBase;

	auto expDir = ntdll.ExportDirectory;

	PDWORD addrOfNameRvas = (PDWORD)(baseNtdll + expDir->AddressOfNames);
	PWORD addrOfOrds = (PWORD)(baseNtdll + expDir->AddressOfNameOrdinals);
	PDWORD addrOfFnRvas = (PDWORD)(baseNtdll + expDir->AddressOfFunctions);

	for (size_t i = 0; i < expDir->NumberOfFunctions; i++)
	{
		LPCSTR fnName = (LPCSTR)(baseNtdll + addrOfNameRvas[i]);
		
		if (strncmp(fnName, "Zw", 2))
			continue;

		WORD fnOrd = addrOfOrds[i];
		DWORD fnRva = addrOfFnRvas[fnOrd];

		if (hash::ror13Hash(fnName) == hash) {
			return (PVOID)((ULONG_PTR)baseNtdll + fnRva);
		}
	}

	return NULL;
}