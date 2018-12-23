# ECM-Sketch
The implementation of ECM-Sketch, an algorithm that allows effective summarization of streaming data over sliding windows with probabilistic accuracy guarantees. 

According to the paper : Sketch-based Querying of Distributed Sliding-Window Data Streams （VLDB 2012）



The file stream.dat is the real IP trace from the main gateway at our campus. This small dataset contains 1M items totally and 193,894 distinct items.



## How to run

Suppose you've already cloned the respository and start from the `ECM-Sketch` directory.

You just need:

```
$ make 
$ ./main
```
