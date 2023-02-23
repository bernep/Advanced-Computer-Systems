# Project 3: Memory and Storage Performance Profiling
## Overview
TBD

## Memory
### Testing Procedure
TBD

### Results
The following results were obtained using `32GB` of `DDR4-3200 RAM`. <br> <br>
![](./lram64.png)
![](./bram64.png)
![](./lram256.png)
![](./bram256.png)

### Analysis
TBD

## Storage
### Testing Procedure
TBD

### Results
The following results were obtained on a `Vansuny USB 3.1 Flash Drive` with `128 GB` of storage using an IO Depth of `16`. It should be noted that this flash drive in particular reports to have `400M B/s` and `240 MB/s` read and write speeds, respectively, which is a fairly standard SSD read/write speed (as of the writing of this, i.e. Feb. 2023). <br>

| Test Type          | Block Size (KB) | IOPS Average | Bandwidth Average (MB/s) | Latency Average (u-sec) |
|--------------------|------------|--------------|--------------------------|-------------------------|
100% Read	| 4	| 275940	| 1056	| 59.8
100% Read	| 32 | 43689 | 1448 | 347
100% Read	| 128	| 11342	| 1473 | 1332
70/30% Read/Write | 4 | 284612 | 997 | 63.3
70/30% Read/Write | 32 | 42597 | 1389 | 362
70/30% Read/Write | 128 | 11058 | 1443 | 1393
100% Write | 4 | 262143 | 1048 | 59.97
100% Write | 32 | 44236 | 1424 | 353.25
100% Write | 128 | 11342 | 1438 | 1399

### Analysis
TBD

## Conclusion
TBD
