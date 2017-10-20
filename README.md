# Repository name: libecg
Hybrid lossless and lossy compression technique for ECG signals

## Publication
Sensing, Signal Processing and Security (ICSSS), 2017 Third International Conference on
[IEEE Xplore](http://ieeexplore.ieee.org/document/8071639/) <http://ieeexplore.ieee.org/document/8071639/>

## Repo Description
Data Compression is the process of identifying existing patterns in the structure of a dataset and eliminating redundancies. There are many ways to perform data compression depending on requirements, application, and needs. In the very general point of view, there are two different types of compression techniques, Lossy and Lossless. Lossless guarantees no loss of data and absolute reconstruction after compression process. Lossy allows data loss up to some degree depending on the type of data and requirements. This research project focuses on a hybrid form of data compression composed of both lossless and lossy approaches.

Different techniques for Electrocardiograph (ECG) data compression are proposed, but yet the performance evaluation and comparison of these techniques is a major difficulty. A unified view of ECG data compression comparison is used to address this issue.

In this research project, a new hybrid data compression technique is proposed for the application of ECG signals. The new hybrid technique provides optional lossy compression layer in order to increase the performance significantly depending on requirements. The technique, without enabling the lossy layer, guarantees no loss of data with low compression ratio. However, by enabling the lossy layer, some signal distortion occurs but compression ratio increases significantly.

The newly proposed technique provides compression ratio up to 10:1 while it introduces signal distortion as low as 0.6 in best case scenario. It is also capable of compressing a specific segment of an ECG signal with no loss of data without requiring any wave recognition algorithm.

A proof-of-concept was necessary in order to measure the actual performance of the proposed compression technique. Due to the flexible nature of the algorithm, it was important to prepare an implementation, preferably as an independent library, so that we could measure the empirical compression performance on a variety range of different ECG signals.

## Design
High-Level Design Model
![High-Level Design Model](https://user-images.githubusercontent.com/29518086/31812365-54e896f0-b5b5-11e7-919a-03def6ff3399.png "High-Level Design Model")
- - - -
Level-1 Design Model: Compression
![Level-1 Design Model: Compression](https://user-images.githubusercontent.com/29518086/31812402-6f5ce7ac-b5b5-11e7-9d4c-298dc19bef7c.png "Level-1 Design Model: Compression")
- - - -
Level-1 Design Model: Decompression
![Level-1 Design Model: Decompression](https://user-images.githubusercontent.com/29518086/31812421-7e322a1c-b5b5-11e7-9c63-0c90c2066315.png "Level-1 Design Model: Decompression")
- - - -
