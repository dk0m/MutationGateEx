# MutationGateEx
Stealthier Implementation Of MutationGate.

## Stop Reading NTDLL From Disk
I have covered the reason why malware developers should stop reading NTDLL from disk in [CraftUnhook](https://github.com/dk0m/CraftUnhook).

To summarize, resolving SSNs has become possible using the exception directory or control flow guard (CFG) function table (See [MDSec's Blog Post](https://www.mdsec.co.uk/2022/04/resolving-system-service-numbers-using-the-exception-directory/)), This eliminates the need for a fresh NTDLL copy.


## Credits
[Resolving System Service Numbers Using The Exception Directory](https://www.mdsec.co.uk/2022/04/resolving-system-service-numbers-using-the-exception-directory/) by [MDSec](https://www.mdsec.co.uk/).

[MutationGate](https://github.com/senzee1984/MutationGate/) by [senzee1984](https://github.com/senzee1984/)
