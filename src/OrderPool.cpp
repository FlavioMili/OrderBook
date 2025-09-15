#include "../include/OrderPool.h"
#include <stdexcept>

OrderPool::OrderPool(size_t size) {
    pool.reserve(size);
    free_list.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        pool.push_back(std::make_unique<Order>());
        free_list.push_back(pool.back().get());
    }
}

Order* OrderPool::allocate(time_t timestamp, OrderSide side, double price, int quantity, int ID) {
    if (free_list.empty()) {
        throw std::runtime_error("OrderPool exhausted");
    }
    Order* order = free_list.back();
    free_list.pop_back();
    
    order->price = price;
    order->timestamp = timestamp;
    order->quantity = quantity;
    order->ID = ID;
    order->side = side;

    return order;
}

void OrderPool::deallocate(Order* order) {
    free_list.push_back(order);
}
