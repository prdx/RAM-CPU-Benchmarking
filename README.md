# Memory Predictor

## Design

The way it works is to keep requesting memory, a page at a time and then by using memset, we will try to access the memory.

Accessing the memory is done by placing a character (one byte) in every address in the page.

```
start = clock();
// Memset here
memset(addr, byte, size);
end = clock();
delta_time = ((double) (end - start));
return delta_time;
```

We are using a list of structs to keep the information of time needed to access the memory.

```
typedef struct __address_info_t {
  double time;
  struct __address_info_t *next;
} address_info_t;
```

Everytime we request a new memory, we will try to record the number of pagefault we get.

We get the number of page fault if the time needed to accessing the same memory region is PF_INDICATOR_FACTOR * the initial time.

```
if (new_time > PF_INDICATOR_FACTOR * prev_time) {
  counter++;
}
```

We use a hyperparameter to determine if we are already running out of memory.
In this case, the hyperparameter is 10 which means if we get 10 consecutive page faults then than means we are already running out of memory.


## Assumption

This program was run on AWS ubuntu machine with 1GB of memory.
No memory exhaustive program was running when we run the experiment.

The output is in byte.

## Preparation

AWS by default does not have swap memory. So we need to define the swap memory, otherwise our process either be killed by the OS or just get stuck.

```
ubuntu@ip-172-31-82-67:~/hw5$ sudo dd if=/dev/zero of=/swap bs=1M count=2048
2048+0 records in
2048+0 records out
2147483648 bytes (2.1 GB, 2.0 GiB) copied, 32.5451 s, 66.0 MB/s
ubuntu@ip-172-31-82-67:~/hw5$ sudo apt-get install neovim
Display all 148 possibilities? (y or n)
ubuntu@ip-172-31-82-67:~/hw5$ sudo mkswap /swap
Setting up swapspace version 1, size = 2 GiB (2147479552 bytes)
no label, UUID=06095ea7-5e05-4597-ade7-c49250b912f9
ubuntu@ip-172-31-82-67:~/hw5$ sudo swapon /swap
swapon: /swap: insecure permissions 0644, 0600 suggested.
```

## Result

In our experiment, we get predicted memory: 943718400

We run the `t2.micro` instance on AWS.

```
ubuntu@ip-172-31-82-67:~/hw5$ free -h
              total        used        free      shared  buff/cache   available
Mem:           990M        131M        740M        164K        118M        721M
Swap:          2.0G         25M        2.0G
```
