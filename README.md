# ECM-Sketch
The implementation of ECM-Sketch, an algorithm that allows effective summarization of streaming data over sliding windows with probabilistic accuracy guarantees. 

According to the paper : Sketch-based Querying of Distributed Sliding-Window Data Streams （VLDB 2012）

The file stream_full.dat is the real IP trace from the main gateway at our campus. This dataset contains 10M items totally and  around 0.1M distinct items. 

## How to run
Suppose you've already cloned the respository and start from the `ECM-Sketch` directory.
You just need:
> $ make
 $ ./main
