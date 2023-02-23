## Project 3: Memory and Storage Performance Profiling
### Overview
TBD

### Results: SSD
The following results were obtained on a <TBD> using an IO Depth of 16.

| Test Type          | Block Size | IOPS Average | Bandwidth Average (MB/s) | Latency Average (u-sec) |
|--------------------|------------|-------|----------------------|--------------------|--------------------|--------------------|-----------------------|---------|---------|-----------|
100% read	| 4 KiB	| 275940	| 1056	| 59.8
100% read	| 32 KiB | 43689 | 1448 | 347
100% read	| 128 KiB	| 11342	| 1473 | 1332
70% read 30% write | 4 KiB | 284612 | 997 | 63.3
70% read 30% write | 32 KiB | 42597 | 1389 | 362
70% read 30% write | 128 KiB | 11058 | 1443 | 1393
100% write | 4 KiB | 262143 | 1048 | 59.97
100% write | 32 KiB | 44236 | 1424 | 353.25
100% write | 128 | KiB | 11342 | 1438 | 1399\
