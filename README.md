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
mkdir build
make
cd build
./OrderBook
```

## Example
This is how the output looks like<br>
<img width="259" alt="Screenshot 2024-12-25 at 16 21 27" src="https://github.com/user-attachments/assets/10034454-7abc-4578-85ec-f4505bb95b82" />


## Performance with bigger example
In the scale of millions of orders it has an average of 600 nanoseconds to process a single order
``` 
Number of orders to process: 1234567
Average time to process an order: 593.097 nanoseconds
Total time: 0.732217 seconds
```
