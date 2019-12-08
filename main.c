#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 5
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 2
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = (int)(long) tid;
    for(int i = 0; i < ORDERS_PER_CUSTOMER; i++){
      Order *curr_order = (Order*) malloc(sizeof(Order));
      MenuItem item = PickRandomMenuItem();
      
      curr_order->customer_id = customer_id;
			curr_order->menu_item = item;
      curr_order->next = NULL;
      
      int order_number = AddOrder(bcb, curr_order);
      printf("Customer with id:#%d added order #%d\n", customer_id, order_number);
    }
	return NULL;
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = (int)(long) tid;
    int orders_fulfilled = 0;
    Order *curr_order; 
    while((curr_order = GetOrder(bcb)) != NULL){
      printf("Cook with id:#%d fulfilled %d orders\n", cook_id, orders_fulfilled);
      free(curr_order); 
      orders_fulfilled++;
    }
    return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
    
    pthread_t customer[NUM_CUSTOMERS]; 
    int customer_id[NUM_CUSTOMERS];
  
    for(int i = 0; i < NUM_CUSTOMERS; i++){ 
      customer_id[i] = i+1;
      pthread_create(&(customer[i]), NULL, BENSCHILLIBOWLCustomer, &(customer_id[i]));
    }
   
	  pthread_t cook[NUM_COOKS];  
    int cook_id[NUM_COOKS];
    
    for (int i = 0; i < NUM_COOKS; i++){ 
        cook_id[i] = i+1;
        pthread_create(&(cook[i]), NULL, BENSCHILLIBOWLCook, &(cook_id[i]));
      
    }
    for(int i = 0; i< NUM_CUSTOMERS; i++){
      pthread_join(customer[i], NULL);
    }
    for (int i = 0; i< NUM_COOKS; i++){
      pthread_join(cook[i], NULL);
    }
    CloseRestaurant(bcb);
    return 0;
}