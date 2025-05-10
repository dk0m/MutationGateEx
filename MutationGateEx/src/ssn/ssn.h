#pragma once

#include<vector>
#include<optional>
#include<winternl.h>

#include "./pe/Pe.h"

// fn hash -> ssn

enum PopulationMethod {
	Rtf, // exception directory (preferred)
	Cfg // control flow guard table (less preferred)
};

typedef struct SSNEntry {
	uint32_t hash;
	uint32_t ssn;
} SSNEntry;

namespace hash {
	uint32_t ror13(uint32_t functionHash, int bits);
	uint32_t ror13Hash(LPCSTR functionName);
}

namespace ssn {
	static Pe ntdll = ParsePeImage("NTDLL");

	static SSNEntry gState{ 0 };

	static std::vector<SSNEntry> entries;

	SSNEntry getState();
	void setState(SSNEntry& entry);

	void rtfPopulateEntries();
	void cfgPopulateEntries();

	void populateEntries(PopulationMethod method);
	std::optional<SSNEntry> findEntryByHash(uint32_t hash);
	PVOID getNtFunctionByHash(uint32_t hash);
}