TWO PLC COMMUNICATION

/* reading s7200 st30 and s71215 both db from it
 *  
 *  Use Semaphore here it is working good.
 *  for making s7 communicatation better without an error
 *  
 *  Also reading from st 30 and sending over to s71200 vice versa.
 *  
*** USE THIS CODE ALWAYS

WITH OUT MQTT HERE.

BUT YOU CAN CREATE TASK FOR MQTT 

There are three task 

1. getting data from both plc
2. sending data to mqtt
3. loop code in task 3
