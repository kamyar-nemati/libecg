# Repository name: libecg
Hybrid lossless and lossy compression technique for ECG signals

## Publication
__Abstract:__ A single cycle of an ECG signal is composed of multiple segments. The QRS segment is considered as the most important segment for accurate diagnosis in many heart related disorders and this segment should be preserved against any major signal distortion during the process of compression. In this paper, a novel hybrid ECG signal data compression technique is proposed, in which lossless compression is applied on QRS segments and lossy compression is applied on other segments, without actually implementing any wave-recognition algorithm. Experimental results have shown that with the optimal selection of threshold and aperture size, it is possible to preserve the quality of QRS segments for enhancing the diagnostic capability with the reconstructed signal while achieving higher compression efficiency at the same time.

_K. Nemati and K. Ramakrishnan, "Hybrid lossless and lossy compression technique for ECG signals," 2017 Third International Conference on Sensing, Signal Processing and Security (ICSSS), Chennai, India, 2017, pp. 450-455._

_doi: 10.1109/SSPS.2017.8071639_

_keywords: {Biomedical signal processing;Data compression;Electrocardiography},_

URL: [http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8071639&isnumber=8071554](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8071639&isnumber=8071554)

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
