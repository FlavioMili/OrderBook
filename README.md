# OrderBook Simulation

A C++ implementation of a simple Order Book system designed to simulate market orders (buy and sell), process matching orders, and track the state of the book.

## Features

- Simulate random buy and sell orders.
- Efficient order processing based on price.
- Dynamic order book updates with matching of buy and sell orders.
- Command-line histogram visualization of the order book state.

## Installation

```bash
git clone https://github.com/FlavioMili/OrderBook.git
cd OrderBook
make
cd build
./OrderBook
```

## Example
This is how the output looks like<br>
<img width="1030" height="614" alt="image" src="https://github.com/user-attachments/assets/228436e7-3728-4be7-9099-a2757952deb3" />



## Performance with bigger example
In the scale of tens of millions of orders it takes an average of 193 nanoseconds to process a single order
``` 
Number of orders to process: 12345678
Average time to process an order: 193.859 nanoseconds
Total time to process 12345678 orders: 2.39332 seconds
```
