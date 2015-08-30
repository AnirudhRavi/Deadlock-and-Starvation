**Program**: Deadlock and Starvation related problem

**Author**: Anirudh Ravi

**About**:

Creating a program using POSIX threads having M threads running at different priorities. There are files (say N files with M >N) which are shared among these M threads. If one thread is using the file (assuming that it is writing onto the file), no other thread will be allowed to use it. The code for the different threads maintains a Table that tells which files have been acquired and for which file requests are
pending. 

While holding a file and requesting for another one, it may so happen that a deadlock occurs: say thread1 has acquired file1 and file2 and requesting for file3 which is already acquired by thread2 which is waiting for file1. Now, an additional thread is created (deadlock detector) which checks for deadlock every 10 seconds and if deadlock is detected it will use deadlock recovery mechanism to break the deadlock. The processing of the file involves appending a line including the thread number to the file and a timestamp. If the file is held by a thread for 6 seconds, 6 such lines appear.

The usage time for a file by a thread is randomly chosen between 2-6 seconds. On an average each running thread will request a file after running between (3*M)/N to (7*M)/N seconds. Moreover the Deadlock detector thread detects starvation such that no thread has an outstanding
request for more than 300 seconds. To avoid starvation, the detector thread will increase the priority of the starved thread to the maximum priority. After the thread is out of starvation original priority is restored. 

**Executing**:

cc Deadlock-and-Starvation.c -lpthread -lrt -o DS

./DS
