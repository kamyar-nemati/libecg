# Repository name: libecg
Hybrid lossless and lossy compression technique for ECG signals

## Publication
Sensing, Signal Processing and Security (ICSSS), 2017 Third International Conference on
[IEEE Xplore](http://ieeexplore.ieee.org/document/8071639/) <http://ieeexplore.ieee.org/document/8071639/>

## Repo Description
A proof-of-concept was necessary in order to measure the actual performance of the proposed compression technique. Due to the flexible nature of the algorithm, it was important to prepare an implementation, preferably as an independent library, so that we could measure the empirical compression performance on a variety range of different ECG signals.

## Design
High-Level Design Model
![High-Level Design Model](https://user-images.githubusercontent.com/29518086/31812365-54e896f0-b5b5-11e7-919a-03def6ff3399.png "High-Level Design Model")

Level-1 Design Model: Compression
![Level-1 Design Model: Compression](https://user-images.githubusercontent.com/29518086/31812402-6f5ce7ac-b5b5-11e7-9d4c-298dc19bef7c.png "Level-1 Design Model: Compression")

Level-1 Design Model: Decompression
![Level-1 Design Model: Decompression](https://user-images.githubusercontent.com/29518086/31812421-7e322a1c-b5b5-11e7-9c63-0c90c2066315.png "Level-1 Design Model: Decompression")
