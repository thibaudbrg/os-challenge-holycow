### Group Holycow 955648 - Operating Systems

___________________________________________________________________________

# Final Submission

The following are what is implemented in our final submission to the OS challenge:

- Multithreading with a pool-thread (6 threads)
- Priority Cost function for scheduling tasks
- Hash-tables for caching

And below are the others features which were fast enough to keep them for the final implementation:

- ??
- ??
- ??
- ??

##### Hardware Specification

All tests have run on the same computer (using Vagrant). The specifications of the computer is listed below:

| Specification    |     Value     |
|------------------|:-------------:|
| CPU              |   Intel i7    |
| CPU clock speed  |    2.5 GHz    |
| No. of CPU cores |       4       |
| RAM amount       |     16 GB     |
| RAM type         | 1600 MHz DDR3 |
| OS               | macOS 10.14.6 |

Although all tests have been conducted on the same machine there is still the
possibility for errors coming from background OS tasks. The tests were run back-to-back.

# Experiments

## Sequential Model vs Multithread Model

##### Author: Thibaud Bourgeois, s221592

##### Branch: MultiThread-Basic, Thread-Pool

#### Experiment Motivation

This overall experiment seeks to figure out how much (if any) we can benefit from multithreading, i.e. handling
multiple client request simultaneously with threads.
The reason we at all bother to do these experiments is because the computer which are running the test have a multi-core
CPU with 4 cores. Splitting the workload between these 4 cores should result in a speed boost.

It was clear that we needed to use a first parallelization method to drastically improve the performance of our server.
To start, we decided to implement a multi-threaded version of the server.

In this experiment, we want to compare an implementation using only one process (sequential) and an implementation using
multiple threads. The motivation for this experiment is to solve the weakness described above.
The thread model is somehow similar to the process model however threads are more lightweight than processes and thus
changing threads is much faster than changing processes and creating and terminating threads is also much faster.
Furthermore, threads share the same address space as opposite to processes.

The implementation of multithreading has been done in several parts to improve its performance and to make it safe:

###### Pool thread

First of all, we had to limit the number of threads created to limit the use of the CPU. Moreover, an infinite number of
threads does not help anymore at a certain point. This is why the creation of a pool-thread was necessary. It
ensures we don't create an unbounded number of threads.
we used first in first out (FIFO) scheduling to start and wait for threads, i.e. when we had started the maximum number
of threads we called join() on the thread which was first started, and started the new thread when join() returned.

###### Thread-Safe Implementation (Locks and Semaphore)

But even if we got comfortable performances (see results below), we got lucky. There was still a major bug in the code.
The shared data structure we created to stack the connection (the queue) was not thread safe: we were still subject to
race conditions.
Hence, we needed to protect those calls (enqueue() and dequeue()) with the help of a mutex lock.

###### Notes about condition variables and semaphores

At the beginning of the experiment I had added (wrongly) variable conditions to avoid threads doing busy
waiting on the queue while waiting for it to fill up. But in our situation this was not really useful
since when threads freed CPU, they had no one to share it with since they were all in the same waiting situation. This
would have worked if several threads were actually working on a common work.

#### Setup

All tests regarding this experiment has been executed on the same computer,
we have tried to keep the workload constant during the test session,
however this is nearly impossible and background activity could result in errors.
The configuration parameters of the client was the following:

##### Run Configuration

| Setting            |     Value      |
|--------------------|:--------------:|
| SERVER             | 192.168.101.10 |
| PORT               |      5003      |
| SEED               |    3435245     |
| TOTAL              |      100       |
| START              |       0        |
| DIFFICULTY         |    30000000    |
| REP\_PROB\_PERCENT |       20       |
| DELAY_US           |     600000     |
| PRIO_LAMBDA        |      1.50      |

#### Results

Below are the results of the tests:

| Run         |   Sequential   |    Threads     |
|-------------|:--------------:|:--------------:|
| First run   |   95.220.820   |   24.671.300   |
| Second run  |   97.668.541   |   25.518.803   |
| Third run   |   92.979.926   |   24.280.959   |
| **Average** | **95.289.762** | **24.823.687** |

#### Discussion and Conclusion

Looking at the results there is a noticeable difference when comparing the averages, the multi-thread model is clearly
faster that the sequential model.
However, there are still some problem with the thread model.
It starts more threads than we have CPU cores, which result in context switches.
We will now try to fix the problem with having more threads than CPU cores.

## Optimizing Maximum Number Threads

###### Author: Thibaud Bourgeois s221592

###### Branch: MultiThread-Basic, Thread-Pool

In this experiment we want to address the problem with having more threads than CPU cores, the reason that this is a
problem is, that if we start more threads than we have cores, the scheduler have to do context (thread) switches (which
takes time even if thread switching is very efficient and much cheaper than process switching. The motivation is to
avoid making these context switches. This should be as simple as setting the number of threads equal to the number of
cores, however we will try some different number of threads to see which number actually give us the best performance.

##### Run Configuration

| Setting            |     Value      |
|--------------------|:--------------:|
| SERVER             | 192.168.101.10 |
| PORT               |      5003      |
| SEED               |    3435245     |
| TOTAL              |      100       |
| START              |       0        |
| DIFFICULTY         |    30000000    |
| REP\_PROB\_PERCENT |       20       |
| DELAY_US           |     600000     |
| PRIO_LAMBDA        |      1.50      |

#### Results

We did the experiment by changing the maximum number of threads to 3, 4, 5, 6, 7, 8 and 20. The result can be found in
the table below. The listed number of threads is only how many threads that are actively calculating client requests, in
addition to those there are also the main thread which is managing the threads as well as setting up the server
initially.

| Run         | 3 Threads      | 4 Threads      | 5 Threads      | 6 Threads      | 7 Threads      | 8 Threads      | 20 Threads     |
|-------------|----------------|----------------|----------------|----------------|----------------|----------------|----------------|
| First Run   | 29.801.572     | 24.671.300     | 20.899.400     | 19.063.160     | 20.425.256     | 21.532.380     | 23.463.315     |
| Second Run  | 29.603.170     | 25.518.803     | 21.005.840     | 23.730.163     | 20.681.164     | 21.664.842     | 22.048.472     |
| Third Run   | 28.995.181     | 24.280.959     | 20.995.528     | 20.129.427     | 22.083.564     | 21.868.455     | 22.498.278     |
| **Average** | **29.464.635** | **24.818.339** | **20.966.867** | **20.882.172** | **21.050.894** | **21.688.119** | **22.662.390** |

Below is a graphical representation of the results:

![](img_readme/img1.png)

#### Discussion and Conclusion

Looking at the graph the best number of threads seems to be 6. We initially expected that the best number would be
3 (4 including the main thread) because this would match the number of cores, and thereby we would avoid making
expensive context switches.
However, a valid point is that 5 (6 including the main thread) could result in a better performance, because the main
thread is idle most of the time, so it could be worth to switch between the main thread and a thread handling client
request. The reason that 6 is the best is more difficult to answer, a guess could be that it is better to start more
threads because of the FIFO problem described in the "Process Model vs Thread Model" experiment (Idle threads if the
first one takes a lot of time).

## Scheduling Threads with a Priority-based Cost Function

###### Author: Thibaud Bourgeois, s221592

###### Branch: Priority-Queue

In this experiment, the goal is to optimize the score by choosing tasks based on their priority.
At first, we will sort the tasks with the priority value, and then with: (priority)/(end - start)

Elements are put into a priority queue, implemented as a linked list. Worker threads then draw from this queue,
taking the higher value computations first, and run them to completion. For these tests, I set the number of
threads in the worker pool-thread to be 6.

##### Run Configuration

| Setting            |     Value      |
|--------------------|:--------------:|
| SERVER             | 192.168.101.10 |
| PORT               |      5003      |
| SEED               |    3435245     |
| TOTAL              |      100       |
| START              |       0        |
| DIFFICULTY         |    30000000    |
| REP\_PROB\_PERCENT |       20       |
| DELAY_US           |     600000     |
| PRIO_LAMBDA        |      1.50      |

#### Results

| Run        | FIFO     | Priority-Queue sorted by taking into account p | Priority-Queue sorted by taking into account start, end and p |
|------------|----------|------------------------------------------------|---------------------------------------------------------------|
| First Run  | 19063160 | 18648245                                       | 17955248                                                      |
| Second Run | 23730163 | 19413889                                       | 17968412                                                      |
| Third Run  | 20129427 | 18515116                                       | 18045548                                                      |
| Average    | 20882172 | 18854961.44                                    | 17989691.95                                                   |

Below is a graphical representation of the results:

![](img_readme/img2.png)

Using the 6 threads pool-thread, we achieve a noticeably better score than the FIFO.
Moreover, taking into account the distance between the start and the end also allows to gain slightly in performance.

#### Discussion

Due to the structure of the priority queue - a linked-list - this code is most efficient when the requests are coming
at approximately the speed they can be solved or slower. If requests come too fast, then each request needs to be
inserted into the linked list individually, leading to O(n^2) performance of insertion sort. This overhead bogs down the
main thread and might interfere with network receive operations.

#### Conclusion

This result of this experiment is as expected - a more intelligent scheduling algorithm
outperforms a simple FIFO.


## Multiprocess Model vs Multithread Model

##### Author: Ghalia Bennani, s221649

##### Branch: Multiprocess-Basic

#### Experiment Motivation

The aim of this experiment is to test another type of parallelization. Indeed, we want to compare the concurrent
implementations using multiples threads and multiple processes. Even if we believe that the multiprocess version will 
be slower than the multi-threads one, we want to give it a try.
The process model works that way: every time the server receives a client request a new process is created using the 
fork system call. We need to choose wisely the number of processes that can live simultaneously, in our case it is four
since the computer which is running the test have a multi-core CPU with 4 cores. This way there is no shared memory 
(critical sections) and thus no need for synchronisation. If we want to increase the number of processes we will need
to protect the critical sections. We will see later what is the optimal choice.

## Optimizing Maximum Number Threads
In this section we seek to find the optimal number of processes.

#### Setup

All tests regarding this experiment has been executed on the same computer,
we have tried to keep the workload constant during the test session,
however this is nearly impossible and background activity could result in errors.
The configuration parameters of the client was the following:

##### Run Configuration

| Setting            |     Value      |
|--------------------|:--------------:|
| SERVER             | 192.168.101.10 |
| PORT               |      5003      |
| SEED               |    3435245     |
| TOTAL              |      100       |
| START              |       0        |
| DIFFICULTY         |    30000000    |
| REP\_PROB\_PERCENT |       20       |
| DELAY_US           |     600000     |
| PRIO_LAMBDA        |      1.50      |

#### Results

Below are the results of the tests:


| Run         | 4 Processes    | 4 Threads      | 5 Threads      | 6 Threads      | 7 Threads      | 8 Threads      | 20 Threads     |
|-------------|----------------|----------------|----------------|----------------|----------------|----------------|----------------|
| First Run   | 43.490.995     | 24.671.300     | 20.899.400     | 19.063.160     | 20.425.256     | 21.532.380     | 23.463.315     |
| Second Run  | 29.603.170     | 25.518.803     | 21.005.840     | 23.730.163     | 20.681.164     | 21.664.842     | 22.048.472     |
| Third Run   | 28.995.181     | 24.280.959     | 20.995.528     | 20.129.427     | 22.083.564     | 21.868.455     | 22.498.278     |
| **Average** | **29.464.635** | **24.818.339** | **20.966.867** | **20.882.172** | **21.050.894** | **21.688.119** | **22.662.390** |






## Caching the requests

##### Author: Ons Riahi, s221565

##### Branch: hashTables

#### Experiment Motivation