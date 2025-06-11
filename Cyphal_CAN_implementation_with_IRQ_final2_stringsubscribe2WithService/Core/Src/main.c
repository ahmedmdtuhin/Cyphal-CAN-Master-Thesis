/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is implemented independently. This is nether generated nor edited from ST's
  * software developed in 2022 (line 16 to 22). Indeed ST's software inspired a lot to understand and implement the code structure
  * in a much better way along with the provided example of libcanard repository.
  *
  * "<h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause"
  *
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "libcanard/canard.h"
#include "uavcan/node/Heartbeat_1_0.h"
#include "uavcan/primitive/array/Real64_1_0.h"
#include "uavcan/primitive/String_1_0.h"
#include "uavcan/_register/List_1_0.h"
#include "uavcan/_register/Name_1_0.h"




/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */



//Canard Instance and transmission queue creation
CanardInstance canard;
CanardTxQueue txqueue;


//For hbeat message transfer id and uptime init
static uint8_t hbeat_message_transfer_id = 0;
uint32_t uptime_s = 0;

//For string
CanardPortID const STRING_MSG_PORT_ID = 2000U;

static uint8_t string_transfer_id = 0;

//Buffer for hbeat message serialization
size_t hbeat_buf_size = uavcan_node_Heartbeat_1_0_EXTENT_BYTES_;
uint8_t hbeat_buf[uavcan_node_Heartbeat_1_0_EXTENT_BYTES_];

//Message portID for Real64
CanardPortID const REAL_MSG_PORT_ID   = 1200U;

//uavcan_primitive_array_Real64_1_0 type array and buffer
uavcan_primitive_array_Real64_1_0 array;
size_t array_buf_size = uavcan_primitive_array_Real64_1_0_EXTENT_BYTES_;


// Example of some register names for Register list service
static const char* g_register_names[] = {
    "uavcan.node.id",     // Possibly link this to node ID
    "uavcan.node.name"  // Store node's name

};

// Calculates how many reg we have
#define NUM_REGISTERS (sizeof(g_register_names) / sizeof(g_register_names[0]))



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_I2C1_Init(void);


/* USER CODE BEGIN PFP */
void CAN_Filter_Config(void);
//Wrappers from standard C (malloc and calloc) for using it 01heap allocator with canard
static void* memAllocate(CanardInstance* const abc, const size_t amount);
static void memFree(CanardInstance* const abc, void* const pointer);
//To process hbeat message transmission queue
void canard_TX_queue_process_func();
uint32_t timer_ms(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  //ssd1306_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  CAN_Filter_Config();
  /* USER CODE END 2 */

  if(HAL_CAN_Start(&hcan1) != HAL_OK) //Starts CAN1
      {
          Error_Handler();
      }

  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); // Enable CAN1 interrupt call backs for the respective interrupt notifications flags



// Very first step: Initialization of canard instance and memory for queue
   canard = canardInit(&memAllocate, &memFree);
   canard.node_id = 17;

   txqueue = canardTxInit( 100,                 		  // Limit the size of the queue at 100 frames.
                           CANARD_MTU_CAN_CLASSIC);       // For classic CAN


//Before receiving a specific UAVCAN subject (e.g., subject-ID 1200 for uavcan.primitive.array.Real64.1.0),
//It must subscribe to that subject first.
//If id does’t subscribe, canardRxAccept() will discard the frames
//because it doesn’t know that we are interested in them .

//To tell which type of message it should subscribe
   CanardRxSubscription subscription; // Transfer subscription state. Structure that will hold the subscription state for one subject.

     if( canardRxSubscribe((CanardInstance *const)&canard,
                           CanardTransferKindMessage,
                           REAL_MSG_PORT_ID,
                           uavcan_primitive_array_Real64_1_0_EXTENT_BYTES_,
                           CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC,
                           &subscription) != 1 )
                           {
                             Error_Handler();
                           }



     //Subscription for string from other node
          CanardRxSubscription sub_string;

          if( canardRxSubscribe((CanardInstance *const)&canard,
                                    CanardTransferKindMessage,
									STRING_MSG_PORT_ID,
     							   uavcan_primitive_String_1_0_EXTENT_BYTES_,
                                    CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC,
                                    &sub_string) != 1 )
                                    {
                                      Error_Handler();
                                    }



     uint32_t last_heartbeat_ms = 0;
     uint32_t last_string_ms    = 0;


     CanardRxSubscription reg_list_svc_sub; // Service subscription state

     // Register list service request acceptance
     if (canardRxSubscribe(
             &canard,
             CanardTransferKindRequest,
             uavcan_register_List_1_0_FIXED_PORT_ID_,   // 385 from the DSDL
             uavcan_register_List_Request_1_0_EXTENT_BYTES_,
             CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC,
             &reg_list_svc_sub) < 0)
     {
         Error_Handler();
     }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // 1) Publish heartbeat every 1 second
	          if (HAL_GetTick() - last_heartbeat_ms >= 1000)
	          {
	        	  last_heartbeat_ms = HAL_GetTick();
	              publishHeartbeat();
	              uptime_s++; // increment for next heartbeat
	          }

	          // 2) If you want to publish a string every 2 seconds
	          if (HAL_GetTick() - last_string_ms >= 1000)
	          {
	              last_string_ms = HAL_GetTick();
	              publishStringMessage("Hello from STM32!");
	              HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	          }

	          // 3) Process TX queue
	          canard_TX_queue_process_func();

	          // 4) Wait a bit
	          HAL_Delay(10);
	      }


	      }
	      /* USER CODE END 3 */

 void publishHeartbeat(void){

	  //For hbeat message transmission

	  	  // 1. Create a heartbeat message
	  	  uavcan_node_Heartbeat_1_0 hbeat_message = { .uptime = uptime_s,
	  	  											  .health = {uavcan_node_Health_1_0_NOMINAL},
	  	  											  .mode   = {uavcan_node_Mode_1_0_OPERATIONAL}
	  	      };

	  	  // 2. Serialize the heartbeat
	  	      if (uavcan_node_Heartbeat_1_0_serialize_(&hbeat_message, hbeat_buf, &hbeat_buf_size) < 0)
	  	      {
	  	        Error_Handler();
	  	      }

	  	  // 3. Create metadata describing this message transfer
	  	      const CanardTransferMetadata transfer_metadata = {  .priority       = CanardPriorityNominal,
	  															  .transfer_kind  = CanardTransferKindMessage,
	  															  .port_id        = uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_,
	  															  .remote_node_id = CANARD_NODE_ID_UNSET,
	  															  .transfer_id    = hbeat_message_transfer_id,
	  	      };

	  	  // 4. Enqueue the message transfer into the TX queue
	  	        if (canardTxPush(&txqueue,
	  	                         &canard,
	  	                         0,                  // No specific deadline
	  	                         &transfer_metadata,
	  	                         hbeat_buf_size,
	  	                         hbeat_buf) < 0)
	  	        {
	  	          Error_Handler();
	  	        }

		        hbeat_message_transfer_id ++;        // transfer increments for hbeat message



  }
  // Implementation of publishStringMessage
  void publishStringMessage(const char* text)
  {
     uavcan_primitive_String_1_0 msg;
     uavcan_primitive_String_1_0_initialize_(&msg);

     size_t length = strlen(text);
     if (length > uavcan_primitive_String_1_0_value_ARRAY_CAPACITY_)
     {
         length = uavcan_primitive_String_1_0_value_ARRAY_CAPACITY_;
     }
     memcpy(msg.value.elements, text, length);
     msg.value.count = length;

     uint8_t ser_buf[uavcan_primitive_String_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_] = {0};
     size_t  ser_size = sizeof(ser_buf);

     if (uavcan_primitive_String_1_0_serialize_(&msg, ser_buf, &ser_size) < 0)
     {
         return; // or Error_Handler();
     }

     const CanardTransferMetadata metadata = {
         .priority       = CanardPriorityNominal,
         .transfer_kind  = CanardTransferKindMessage,
         .port_id        = STRING_MSG_PORT_ID,
         .remote_node_id = CANARD_NODE_ID_UNSET,
         .transfer_id    = string_transfer_id++,
     };

     if (canardTxPush(&txqueue, &canard, 0, &metadata, ser_size, ser_buf) < 0)
     {
    	 Error_Handler();// handle error
     }


}

  //Register name
  static void setRegisterName(uavcan_register_Name_1_0* out_name,
                              const char* reg_name_str)
  {
      // Zero-initialize first
      uavcan_register_Name_1_0_initialize_(out_name);

      // Copy up to the capacity
      size_t str_len = strlen(reg_name_str);
      if (str_len > uavcan_register_Name_1_0_name_ARRAY_CAPACITY_)
      {
          str_len = uavcan_register_Name_1_0_name_ARRAY_CAPACITY_;
      }
      memcpy(out_name->name.elements, reg_name_str, str_len);
      out_name->name.count = str_len;
  }
  //Register service
  static void handleRegisterListRequest(const CanardRxTransfer* transfer)
  {
      // 1) Deserialize the request
      uavcan_register_List_Request_1_0 req;
      size_t req_buf_size = transfer->payload_size;

      if (uavcan_register_List_Request_1_0_deserialize_(&req, transfer->payload, &req_buf_size) < 0)
      {
          printf("Failed to deserialize uavcan.register.List.Request\n");
          return; // or Error_Handler();
      }

      // 2) Prepare the response
      uavcan_register_List_Response_1_0 resp;
      uavcan_register_List_Response_1_0_initialize_(&resp);

      // If index is valid, set the corresponding register name; otherwise empty
      if (req.index < NUM_REGISTERS)
      {
          // Convert our register name to the `uavcan.register.Name.1.0` format
          // Typically, you'd do something like:
          setRegisterName(&resp.name, g_register_names[req.index]);
          // We'll define `setRegisterName()` below
      }
      else
      {
          // Index out of range => name is empty
          // uavcan_register_Name_1_0_initialize_(&resp.name);
          // The `initialize_()` call is already done, so it’s empty by default
      }

      // 3) Serialize the response
      uint8_t resp_buf[uavcan_register_List_Response_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_] = {0};
              size_t  resp_buf_size = sizeof(resp_buf);

              if (uavcan_register_List_Response_1_0_serialize_(&resp, resp_buf, &resp_buf_size) < 0)
              {
                  printf("Failed to serialize uavcan.register.List.Response\n");
                  return;
              }

      // 4) Send the response
      CanardTransferMetadata metadata = {
          .priority       = transfer->metadata.priority, // Usually same priority
          .transfer_kind  = CanardTransferKindResponse,
          .port_id        = uavcan_register_List_1_0_FIXED_PORT_ID_, // 385
          .remote_node_id = CANARD_NODE_ID_UNSET,       // Respond to requestor
          .transfer_id    = transfer->metadata.transfer_id,
      };

      if (canardTxPush(&txqueue, &canard, 0, &metadata, resp_buf_size, resp_buf) < 0)
      {
          Error_Handler();
      }
  }

void canard_TX_queue_process_func ()
{
	// Look at the top of the TX queue
	  for (const CanardTxQueueItem* ti = NULL; (ti = canardTxPeek(&txqueue)) != NULL;)
	  {
	    // 1. Check if the TX deadline has not passed
	    if ((0U == ti->tx_deadline_usec) || (ti->tx_deadline_usec > timer_ms()))
	    {
	      // 2. Prepare the CAN header for transmission

	      CAN_TxHeaderTypeDef TxHeader;
	      TxHeader.IDE = CAN_ID_EXT;             // Use extended CAN ID
	      TxHeader.RTR = CAN_RTR_DATA;           // Data frame
	      TxHeader.DLC = ti->frame.payload_size; // Number of bytes in payload
	      TxHeader.ExtId = ti->frame.extended_can_id;

	      uint8_t TxData[8];
	      uint32_t TxMailbox;

	      // 3. Copy the payload into a temporary buffer
	      memcpy(TxData, (uint8_t *)ti->frame.payload, ti->frame.payload_size);

	      // 4. Transmit using the HAL driver

	      if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	      {
	        // If transmission fails (for example, no empty mailbox), break and try again later
	        break;
	      }
	    }

	    // 5. Whether sent or expired, pop from the queue and free memory
	    canard.memory_free(&canard, canardTxPop(&txqueue, ti));
	  }
}

//Callback function for each CAN message receiption in fifo0
/* main.c */

/**
 * @brief CAN RX FIFO 0 message pending callback.
 * @param hcan Pointer to the CAN handle.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    // 1. Read the CAN frame from FIFO0
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8] = {0};
    uint32_t RxFifo0Index = CAN_RX_FIFO0;  // Typically 0

    if (HAL_CAN_GetRxMessage(hcan, RxFifo0Index, &RxHeader, RxData) != HAL_OK)
    {
        printf("Failed to get CAN RX message.\n");
        return;
    }

    // 2. Construct a CanardFrame
    CanardFrame frame;
    frame.extended_can_id = RxHeader.ExtId;
    frame.payload_size = (size_t)RxHeader.DLC;
    frame.payload = (void*)RxData;

    // 3. Attempt to accept and reassemble the transfer
    CanardRxTransfer transfer;

    if (canardRxAccept(&canard,
                       timer_ms(),
                       &frame,
                       0,
                       &transfer,
                       NULL) != 1)
    {
        // The frame received is not a valid transfer
        printf("Invalid CAN transfer received.\n");
        return;
    }

    // 4. Determine the transfer kind and port ID
    const CanardPortID rx_port_id = transfer.metadata.port_id;
      if ((transfer.metadata.transfer_kind == CanardTransferKindRequest) &&
          (rx_port_id == uavcan_register_List_1_0_FIXED_PORT_ID_))  // 385
      {
          handleRegisterListRequest(&transfer);
      }

      else if (transfer.metadata.transfer_kind == CanardTransferKindMessage)
    {
        // Handle messages (Real64 and String)
        if (rx_port_id == REAL_MSG_PORT_ID)
        {
            // Deserialize and handle Real64 message
            uavcan_primitive_array_Real64_1_0 array_msg;
            size_t array_buf_size = uavcan_primitive_array_Real64_1_0_EXTENT_BYTES_;
            if (uavcan_primitive_array_Real64_1_0_deserialize_(&array_msg, transfer.payload, &array_buf_size) < 0)
            {
                printf("Failed to deserialize Real64 message.\n");
                Error_Handler();
            }

            // Debug array content via UART
            for (size_t i = 0; i < array_msg.value.count; i++) {
                printf("Element %zu: %.2f\n", i, array_msg.value.elements[i]);
            }

            // Prepare message for UART
            char msg[128] = {0};
            size_t len = 0;

            len += snprintf(msg + len, sizeof(msg) - len, "Real64 Message Received: ");
            for (size_t i = 0; i < array_msg.value.count; i++) {
                len += snprintf(msg + len, sizeof(msg) - len, "%.2f ", array_msg.value.elements[i]);
                if (len >= sizeof(msg)) break; // Prevent buffer overflow
            }
            len += snprintf(msg + len, sizeof(msg) - len, "\r\n");

            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

            // Prepare message for OLED
            #define MAX_OLED_WIDTH 21 // Characters per line for 128-pixel-wide display with Font_6x8
            #define MAX_LINES 4       // Adjust based on OLED height and font size

            char lines[MAX_LINES][MAX_OLED_WIDTH + 1] = {0}; // Buffer for multiple lines

            // Split data into multiple lines and check for special values
            size_t index = 0;
            bool motor_on = false, motor_off = false; // Flags for Motor On/Off

            for (size_t i = 0; i < array_msg.value.count; i++) {
                char temp[16];
                snprintf(temp, sizeof(temp), "%.1f ", array_msg.value.elements[i]); // Format each value
                if (array_msg.value.elements[i] == 1.1) {
                    motor_on = true; // Set Motor On flag
                } else if (array_msg.value.elements[i] == 2.2) {
                    motor_off = true; // Set Motor Off flag
                }

                if (strlen(lines[index]) + strlen(temp) < MAX_OLED_WIDTH) {
                    strcat(lines[index], temp); // Add to current line
                } else if (index < MAX_LINES - 1) {
                    index++; // Move to next line
                    strcat(lines[index], temp); // Start new line
                } else {
                    break; // Stop if no more lines are available
                }
            }

            // Display on OLED
            ssd1306_Fill(Black);
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString("Real64 Message:", Font_6x8, White);

            for (size_t i = 0; i <= index; i++) {
                ssd1306_SetCursor(0, 16 + (i * 8)); // Moved to next line (adjusted line height as needed)
                ssd1306_WriteString(lines[i], Font_6x8, White);
            }

            // Add Motor On/Off message if flags are set
            if (motor_on) {
                ssd1306_SetCursor(0, 16 + ((index + 1) * 8)); // Extra line for Motor On
                ssd1306_WriteString("Motor On", Font_6x8, White);
            } else if (motor_off) {
                ssd1306_SetCursor(0, 16 + ((index + 1) * 8)); // Extra line for Motor Off
                ssd1306_WriteString("Motor Off", Font_6x8, White);
            }

            ssd1306_UpdateScreen();
        }
        else if (rx_port_id == STRING_MSG_PORT_ID)
        {
            // Deserialize and handle String message
            uavcan_primitive_String_1_0 str_msg;
            size_t str_buf_size = uavcan_primitive_String_1_0_EXTENT_BYTES_;
            if (uavcan_primitive_String_1_0_deserialize_(&str_msg, transfer.payload, &str_buf_size) < 0)
            {
                printf("Failed to deserialize String message.\n");
                Error_Handler();
            }

            // Convert to C-string for printing
            char text[256] = {0}; // Adjust size as needed
            size_t copy_len = (str_msg.value.count < sizeof(text) - 1) ? str_msg.value.count : sizeof(text) - 1;
            memcpy(text, str_msg.value.elements, copy_len);
            text[copy_len] = '\0';

            // Debug String content via UART
            printf("String Message Received: \"%s\"\n", text);

            // Prepare message for UART
            char msg[256] = {0};
            snprintf(msg, sizeof(msg), "String Message: %s\r\n", text);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

            // Prepare message for OLED
            #define MAX_OLED_WIDTH 21 // Characters per line for 128-pixel-wide display with Font_6x8
            #define MAX_LINES 4       // Adjust based on OLED height and font size

            char lines[MAX_LINES][MAX_OLED_WIDTH + 1] = {0}; // Buffer for multiple lines

            // Split the string into multiple lines if necessary
            size_t index = 0;
            size_t len = strlen(text);
            size_t start = 0;

            while (start < len && index < MAX_LINES)
            {
                size_t end = start + MAX_OLED_WIDTH;
                if (end > len)
                    end = len;

                size_t line_length = end - start;
                strncpy(lines[index], &text[start], line_length);
                lines[index][line_length] = '\0'; // Null-terminate

                start += line_length;
                index++;
            }

            // Display on OLED
            ssd1306_Fill(Black);
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString("String Message:", Font_6x8, White);

            for (size_t i = 0; i < index; i++) {
                ssd1306_SetCursor(0, 16 + (i * 8)); // Moved to next line (adjusted line height as needed)
                ssd1306_WriteString(lines[i], Font_6x8, White);
            }

            ssd1306_UpdateScreen();
        }
        else
        {
            printf("Received message on unknown Port ID: %u\n", rx_port_id);
        }
    }

    // 5. Free memory allocated by libcanard
    canard.memory_free(&canard, transfer.payload);

    // 6. Toggle LED to indicate successful reception
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    return;
}



//Wrappers from standard C (malloc and calloc) for using it 01heap allocator with canard

static void* memAllocate(CanardInstance* const abc, const size_t amount)
{
  (void) abc;
  return malloc(amount);
}

static void memFree(CanardInstance* const abc, void* const pointer)
{
  (void) abc;
  free( pointer );
}


uint32_t timer_ms(void) // Need to set up timer peripheral, based on the microsecond precision requirements
{
	return HAL_GetTick();
	//return 0;
}




//ALL the peripheral initialization
/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}





/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

void CAN_Filter_Config(void)
{

	 	CAN_FilterTypeDef can1_filter_init;

	 	can1_filter_init.FilterActivation = ENABLE;
	 	can1_filter_init.FilterBank  = 0;
	 	can1_filter_init.FilterFIFOAssignment = CAN_RX_FIFO0;
	 	can1_filter_init.FilterIdHigh = 0x0000;
	 	can1_filter_init.FilterIdLow = 0x0000;
	 	can1_filter_init.FilterMaskIdHigh = 0X0000;
	 	can1_filter_init.FilterMaskIdLow = 0x0000;
	 	can1_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
	 	can1_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

	 	if( HAL_CAN_ConfigFilter(&hcan1,&can1_filter_init) != HAL_OK)
	 	{
	 		Error_Handler();

	 }

}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
