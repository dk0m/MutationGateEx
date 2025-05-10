#include<windows.h>

#pragma pack(1)
typedef struct _IMAGE_CFG_ENTRY {
	DWORD Rva;
	struct {
		BOOLEAN SuppressedCall : 1;
		BOOLEAN ExportSuppressed : 1;
		BOOLEAN LangExcptHandler : 1;
		BOOLEAN Xfg : 1;
		BOOLEAN Reserved : 4;
	} Flags;
} IMAGE_CFG_ENTRY, * PIMAGE_CFG_ENTRY;

class Pe {
public:
	PVOID ImageBase;
	PIMAGE_DOS_HEADER DosHeader;
	PIMAGE_NT_HEADERS NtHeaders;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
	IMAGE_FILE_HEADER FileHeader;

	PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
	PIMAGE_EXPORT_DIRECTORY ExportDirectory;
	PIMAGE_RUNTIME_FUNCTION_ENTRY RunTimeEntryTable;

	PIMAGE_CFG_ENTRY GuardCffTable;
};

Pe ParsePeImage(LPCSTR imageName);