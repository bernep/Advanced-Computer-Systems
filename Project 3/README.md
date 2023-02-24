# Project 3: Memory and Storage Performance Profiling
## Overview
### Introduction
In this project, we delve into the operating conditions of modern memory and storage devices. Specifically, we will observe the relationship between data throughput and latency using [Intel's Memory Latency Checker](https://www.intel.com/content/www/us/en/developer/articles/tool/intelr-memory-latency-checker.html) for memory, and the [Flexible IO Tester](https://github.com/axboe/fio) library for storage.

### Queuing Theory
Queuing theory essentially refers to the mathematics of waiting lines. There are two main concepts to consider in the case of compute memory/storage: bandwidth and latency. If you were to relate this to a real-life scenario, latency would be equivalent to the amount of time people wait in a line (e.g. waiting in an amusement park line), while bandwidth refers to how long it takes for the people to do the thing they're waiting for (e.g. how long it takes to ride an amusement park ride). In order to maximize the speed you get from a queuing process, you need to optimize both of these parameters. In this project, we look for configurations to optimize speed by finding a good trade-off between bandwidth and latency.




## Testing Environment
#### System
* Framework Laptop
* WSL2 - Ubuntu 22.04 LTS

#### CPU
* Intel® Core™ i7-1165G7 Processor
  - 4 cores
  - 8 threads
  - 2.8 GHz clock
  - 12 MB cache (L1: 96K, L2: 1.25MB, L3: 12MB)
  
#### Memory
* DDR4 SO-DIMM
* 3200 MHz
* 32GB of Memory

#### Storage
* Vansuny USB 3.1 Flash Drive
* 128 GB of Storage
* 400 MB/s Read Speed
* 240 MB/s Write Speed




## Memory
### Testing Procedure
Using [Intel's Memory Latency Checker](https://www.intel.com/content/www/us/en/developer/articles/tool/intelr-memory-latency-checker.html), we ran different configurations for read/write ratios and data access sizes. Specifically, the read/write ratios determined the number of data reads versus non-temporal writes.

### Results
![](./lram64.png)
![](./bram64.png)
![](./lram256.png)
![](./bram256.png)

### Analysis
* As the inject delay increases, the bandwidth decreases for all configurations, which is expected since a larger inject delay leads to more time spent waiting for the data to be retrieved, thus resulting in less data that can be processed.
* Low inject delays have a very large bandwidth but also a very large latency, meaning that the system can process data quickly, but it also has to wait significantly longer to obtain new data. In regards to queuing theory, the ratio of bandwidth to latency here is not ideal for optimal performance.
* Increasing the injection rate ultimately has diminishing returns on reducing latency, since all read/write configurations generally end up having the same plateued latency at high injection levels. Bandwidth, however, continues to decrease significantly as the injection rate increases for all read/write configurations.
* Adjusting the access size doesn't have an extraordinary effect on performance, although the main difference seems to be an increase in latency when increasing access size. Lower access sizes also seem to favor reading configurations more while higher access sizes seem to favor writing configurations more.
* The most optimal results seem to come from a mixture of read/write and a moderate injection delay for both access sizes, which generally makes sense since queuing theory indicates that a balance should be made between bandwidth and latency.




## Storage
### Testing Procedure and Context
Using [Flexible IO Tester](https://github.com/axboe/fio) on an external flash drive, we ran different configurations for read/write ratios and data access sizes. Additionally, an IO Depth of `16` was used. It should be noted that this flash drive in particular reports to have `400M B/s` and `240 MB/s` read and write speeds, respectively, which is a fairly standard SSD read/write speed (as of the writing of this, i.e. Feb. 2023). <br>

### Results
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
#### General Analysis
* In general, when the data access size increases, the IOPS (Input/Output Operations Per Second) decreases while the bandwidth and latency increase. This is because larger block sizes allow for more data to be transferred with each I/O operation, but the amount of time it takes to access and process the larger data blocks also goes up. 
* Adjusting the read/write ratio does not seem to have a major effect on any of the performance metrics, which is odd since the SSD manufacturer reports 240MB/s write versus a 400MB/s read. You would think that the reading configurations would have higher bandwidth and lower latency, but the results are similar.
* It should be noted that only one IO depth (i.e. `16`) was shown here, but in general, increasing IO depth results in higher throughput (bandwidth), but increased latency.
* The trade-off between bandwidth and latency is quite obvious here with data access size, as that is the main factor for performance. Depending on your system, you should adjust this variable to increase/decrease bandwidth/latency.

#### Comparison to Intel Enterprise-Grade SSD
We've been asked to compare our client-grade SSD (128GB) to Intel's entrerprise-grade Data Center NVMe SSD D7-P5600 (1.6TB). Intel's SSD reports a random write-only IOPS of 130K for 4KB access blocks. Our SSD reports 262K IOPS for the same test, which initially seems odd, since you would expect an enterprise-grade SSDs to have a larger IOPS. However, its makes sense when you consider that enterprise-grade SSDs are designed for specific uses. In this case, the enterprise-grade SSD has a reading IOPS of up to 1M, meaning that it's specifically optimized for reading data, whereas my SSD is optimized for general usage (reading and writing).


## Conclusion
In both cases for memory and storage, the optimal speed for each system was achieved when compromosing between bandwidth and latency, which is expected in queuing theory. There are a lot of specifics when it comes to changing read/write speed and data access sizes, but in general, only prioritizing one parameter usually did not lead to an optimal outcome.
