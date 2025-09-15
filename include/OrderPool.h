#ifndef ORDER_POOL_INCLUDED
#define ORDER_POOL_INCLUDED

#include "../include/Order.h"
#include <vector>
#include <memory>

class OrderPool {
public:
    OrderPool(size_t size);
    Order* allocate(time_t timestamp, OrderSide side, double price, int quantity, int ID);
    void deallocate(Order* order);

private:
    std::vector<std::unique_ptr<Order>> pool;
    std::vector<Order*> free_list;
};

#endif // !ORDER_POOL_INCLUDED
