# BMG250 sensor API
## Introduction
This package contains the Bosch Sensortec's BMG250 gyroscope sensor driver (sensor API)

The sensor driver package includes bmg250.c, bmg250.h and bmg250_defs.h files

## Sensor API Revisions
Files         | Revision | Release date
--------------|----------|-------------
bmg250.c      | 1.0.0    | 21 Jun 2017
bmg250.h      | 1.0.0    | 21 Jun 2017
bmg250_defs.h | 1.0.0    | 21 Jun 2017

## Integration details
* Integrate bmg250.h, bmg250_defs.h and bmg250.c file in to your project.
* Include the bmg250.h file in your code like below.
``` c
#include "bmg250.h"
```

## File information
* bmg250_defs.h : This header file has the constants, macros and datatype declarations.
* bmg250.h      : This header file contains the declarations of the sensor driver APIs.
* bmg250.c      : This source file contains the definitions of the sensor driver APIs.

## Supported sensor interfaces
* SPI 4-wire
* I2C

## Usage guide
### Initializing the sensor
To initialize the sensor, you will first need to create a device structure. You 
can do this by creating an instance of the structure bmg250_dev. Then go on to 
fill in the various parameters as shown below.


#### Example for SPI 4-Wire
``` c
struct bmg250_dev gyro;
int8_t rslt = BMG250_OK;

/* Sensor interface over SPI with native chip select line */
gyro.dev_id = 0;
gyro.interface = BMG250_SPI_INTF;
gyro.read = user_spi_read;
gyro.write = user_spi_write;
gyro.delay_ms = user_delay_ms;

rslt = bmg250_init(&gyro);
```
#### Example for I2C
``` c
struct bmg250_dev gyro;
int8_t rslt = BMG250_OK;

/* Sensor interface over I2C */
gyro.dev_id = BMG250_I2C_ADDR;
gyro.interface = BMG250_I2C_INTF;
gyro.read = user_i2c_read;
gyro.write = user_i2c_write;
gyro.delay_ms = user_delay_ms;

rslt = bmg250_init(&gyro);
```
### Sensor Configuration settings
#### Setting sensor in normal power mode
##### Example for configuring the sensor in normal power mode
``` c
int8_t set_power_mode(struct bmg250_dev *dev)
{
	int8_t rslt;

	/* Setting the power mode as normal */
	dev->power_mode = BMG250_GYRO_NORMAL_MODE;
	
	rslt = bmg250_set_power_mode(dev);
	
	return rslt;	
}
```

##### Example for configuring the sensor
>
``` c
int8_t set_sensor_config(struct bmg250_dev *dev)
{
	int8_t rslt;
	/* Structure to set the gyro config */
	struct bmg250_cfg gyro_cfg;

	/* Read the set configuration from the sensor */
	rslt = bmg250_get_sensor_settings(&gyro_cfg, dev);
	
	if (rslt == BMG250_OK) {
		/* Modify your desired configurations , say set the Range into 1000 DPS */
		gyro_cfg.range = BMG250_RANGE_1000_DPS;
	}
	rslt = bmg250_set_sensor_settings(&gyro_cfg, dev);
	
	
	
	return rslt;	
}
```

### Reading sensor data 
> Sensor data should be read after setting the desired power mode, ODR, bandwidth and range.
#### Example for reading sensor data with sensor-time
``` c
int8_t read_sensor_data(struct bmg250_dev *dev)
{
	int8_t rslt;
	
	/* Structure to store the sensor data */
	struct bmg250_sensor_data gyro_data;

	/* Structure to set the gyro config */
	struct bmg250_cfg gyro_cfg;
	
	/* Read the set configuration from the sensor */
	rslt = bmg250_get_sensor_settings(&gyro_cfg, dev);
	
	if (rslt == BMG250_OK) {
		/* Selecting the ODR as 100Hz */
		gyro_cfg.odr = BMG250_ODR_100HZ;
		/* Selecting the bw as Normal mode */
		gyro_cfg.bw = BMG250_BW_NORMAL_MODE;
		/* Selecting the range as 1000 Degrees/second */
		gyro_cfg.range = BMG250_RANGE_1000_DPS;
		
		/* Set the above selected ODR, Range, BW in the sensor */
		rslt = bmg250_set_sensor_settings(&gyro_cfg, dev);
		
		/* Read sensor data */
		if (rslt == BMG250_OK) {
			while (1) {
				rslt = bmg250_get_sensor_data(BMG250_DATA_TIME_SEL, gyro_data, dev);
				printf("Gyro data  X: %d \t Y: %d \t Z: %d \t Sensor-time : %d"
					, gyro_data.x, gyro_data.y, gyro_data.z, gyro_data.sensortime);
					
				/* Delay of 10ms is added since ODR is 100Hz */
				dev->delay_ms(10);
			}	
		}
	}
	
	return rslt;
}
```
#### Example for using data ready interrupts in BMG250
> Configuring data ready interrupt and interrupt pins settings 
``` c
int8_t drdy_overflow_int_setting(struct bmg250_dev *dev)
{
	int8_t rslt;
	/* Interrupt setting structure */
	struct bmg250_int_settg int_conf;
	uint8_t int_status;

	/* Enable the desired settings to be set in the sensor 
	 * Refer below for all possible settings */
	int_conf.int_channel = BMG250_INT_CHANNEL_1;
	int_conf.int_type = BMG250_DATA_RDY_INT;
	int_conf.int_pin_settg.output_en = BMG250_ENABLE;
	int_conf.int_pin_settg.output_mode = BMG250_PUSH_PULL;
	int_conf.int_pin_settg.output_type = BMG250_ACTIVE_HIGH;
	int_conf.int_pin_settg.edge_ctrl = BMG250_EDGE_TRIGGER;
	int_conf.int_pin_settg.input_en = BMG250_DISABLE;
	
	/* Set the desired configuration in the sensor */
	rslt = bmg250_set_int_config(&int_conf, dev);
	
	/* Data ready interrupt will be asserted henceforth
	 * every time when a new data sample is ready */
}
```
Possible values of macros which can be assigned for interrupt configuration 

	int_conf.int_channel  
		- BMG250_INT_CHANNEL_1  (Maps the interrupt to the INT1 pin of sensor)
                - BMG250_INT_CHANNEL_2  (Maps the interrupt to the INT2 pin of sensor)
		
	int_conf.int_type 
		BMG250_DATA_RDY_INT        (Enables the Data ready interrupt)
		BMG250_FIFO_FULL_INT       (Enables the FIFO full interrupt)
		BMG250_FIFO_WATERMARK_INT  (Enables the FIFO watermark interrupt)
		
	int_conf.int_pin_settg.output_en
		BMG250_DISABLE             (Disables the interrupt pin for interrupt output)
		BMG250_ENABLE              (Enables the interrupt pin for interrupt output)
		
	int_conf.int_pin_settg.output_mode
		- BMG250_PUSH_PULL        (Sets the INT pin as push-pull )
		- BMG250_OPEN_DRAIN       (Sets the INT pin as open drain)
		
	int_conf.int_pin_settg.output_type
		- BMG250_ACTIVE_LOW       (Sets the interrupt signal as active low)
		- BMG250_ACTIVE_HIGH      (Sets the interrupt signal as active high)
		
	int_conf.int_pin_settg.edge_ctrl  
		- BMG250_LEVEL_TRIGGER    (Sets the interrupt as Level_triggered)
		- BMG250_EDGE_TRIGGER     (Sets the interrupt as edge_triggered)
		
	int_conf.int_pin_settg.input_en 
		- BMG250_DISABLE         (Disables the interrupt pin for input)
		- BMG250_ENABLE          (Enables the interrupt pin for input)
		



#### Example for Configuring FIFO and reading data in BMG250
> Reading FIFO data for header 
``` c
	/* Macros to be configured by the user */
/* Buffer size allocated to store raw FIFO data */
#define BMG250_FIFO_RAW_DATA_BUFFER_SIZE        UINT16_C(1000)
/* Length of data to be read from FIFO */
#define BMG250_FIFO_RAW_DATA_USER_LENGTH        UINT16_C(1000)
/* Number of Gyro frames to be extracted from FIFO */
#define BMG250_FIFO_EXTRACTED_DATA_FRAME_COUNT  UINT8_C(200)
/* FIFO data filling delay */
#define BMG250_FIFO_CONFIG_READ_DELAY           UINT8_C(200)

/* Configuring, Reading FIFO and extraction of gyro data */
int8_t read_fifo_data(struct bmg250_dev *dev)
{
	int8_t rslt;
	uint16_t n_instance;
	/* Gyro frames needed from FIFO*/
	uint8_t frames_needed;
	
	/* Setup and configure the FIFO buffer */
	/* Declare memory to store the raw FIFO buffer information */
	uint8_t fifo_data[BMG250_FIFO_RAW_DATA_BUFFER_SIZE] = {0};
	struct bmg250_fifo_frame fifo_frame;
	struct bmg250_sensor_data gyro_data[BMG250_FIFO_EXTRACTED_DATA_FRAME_COUNT] = {0};


	/* Modify the FIFO buffer instance and link to the device instance */
	fifo_frame.data = fifo_data;
	fifo_frame.length = BMG250_FIFO_RAW_DATA_USER_LENGTH;
	dev->fifo = &fifo_frame;

	/* Disable other FIFO configuration settings in the sensor */
	rslt = bmg250_set_fifo_config(BMG250_FIFO_ALL_SETTING, BMG250_DISABLE, dev);
	if (rslt == BMG250_OK) {
		/* Enable required FIFO configuration settings in the sensor */
		rslt = bmg250_set_fifo_config(BMG250_FIFO_GYRO | BMG250_FIFO_HEADER | BMG250_FIFO_TIME, 
						BMG250_ENABLE, dev);
		if (rslt == BMG250_OK) {
			/* FIFO filling delay */
			dev->delay_ms(BMG250_FIFO_CONFIG_READ_DELAY);
			
			/* Read FIFO data */
			printf("\n FIFO data bytes requested  :  %d \n",dev->fifo->length);
			rslt = bmg250_get_fifo_data(dev);
			if (rslt == BMG250_OK) {
				printf("\n FIFO data bytes available  : %d \n",dev->fifo->length);
				for(n_instance = 0; n_instance < dev->fifo->length ; n_instance++) {
					/* Printing the FIFO data */
					printf("\n FIFO DATA [%d]  : %x "
						,n_instance ,dev->fifo->data[n_instance]);
				}
			} else {
				printf("\n FIFO data read failed");
			}
			
			/* Extracting Gyro data */
			frames_needed = BMG250_FIFO_EXTRACTED_DATA_FRAME_COUNT;
			printf("\n FIFO gyro frames requested :  %d \n",frames_needed);
			rslt = bmg250_extract_gyro(&gyro_data[0], &frames_needed, dev);
			if (rslt == BMG250_OK) {
				printf("\n FIFO gyro frames extracted :  %d \n",frames_needed);
				
				/* Print the extraxted gyro data frames */
				for (i = 0; i < frames_needed + 2; i++) {
					printf("\n FIFO DATA [%d] --> X:%d  \t, Y:%d  \t, Z:%d \n", i
						, gyro_data[i].x, gyro_data[i].y, gyro_data[i].z);
				}
				printf("\n gyro_byte_start_idx = %d \n",dev->fifo->gyro_byte_start_idx);
				printf("\n skipped_frame_count = %d \n",dev->fifo->skipped_frame_count);
				printf("\n sensor_time = %d \n",dev->fifo->sensor_time);
			} else {
				printf("\n FIFO data extraction failed");
			}
		}
	}
	
	return rslt;
}
```
#### Example for Configuring FIFO watermark interrupt
> Watermark interrupt is set to trigger after FIFO 
> is filled with 100 frames of gyro data
``` c
	/* Macros to be configured by the user */
/* FIFO FRAMES TO BE FILLED TO GET WATERMARK INERRUPT */
#define BMG250_FIFO_FRAME_FOR_WM_INT            UINT8_C(100)
/* FIFO data filling delay */
#define BMG250_FIFO_CONFIG_READ_DELAY           UINT8_C(200)

/* FIFO Watermark interrupt test */
int8_t set_fifo_wm_interrupt(struct bmg250_dev *dev)
{
	int8_t rslt;
	/* Interrupt setting structure */
	struct bmg250_int_settg int_conf;
	/* Interrupt status */
	uint8_t int_status;
	/* FIFO length in number of bytes to be read after the 
	watermark interrupt is asserted */
	uint16_t fifo_len;

	/* Disable other FIFO configuration settings in the sensor */
	rslt = bmg250_set_fifo_config(BMG250_FIFO_ALL_SETTING, BMG250_DISABLE, dev);
	if (rslt == BMG250_OK) {
		/* Enable required FIFO configuration settings in the sensor */
		rslt = bmg250_set_fifo_config(BMG250_FIFO_GYRO , BMG250_ENABLE, dev);
		if (rslt == BMG250_OK) {	
			int_conf.int_channel = BMG250_INT_CHANNEL_1;
			int_conf.int_type = BMG250_FIFO_WATERMARK_INT;
			int_conf.fifo_wtm_int_en = BMG250_ENABLE;
			int_conf.int_pin_settg.output_en = BMG250_ENABLE;
			int_conf.int_pin_settg.output_mode = BMG250_PUSH_PULL;
			int_conf.int_pin_settg.output_type = BMG250_ACTIVE_HIGH;
			int_conf.int_pin_settg.edge_ctrl = BMG250_EDGE_TRIGGER;
			int_conf.int_pin_settg.input_en = BMG250_DISABLE;
			/* Set the interrupt configuration settings to the sensor */
			rslt = bmg250_set_int_config(&int_conf, dev);
			if (rslt == BMG250_OK) {
				printf("\n FIFO frames requested : 100 ");
				rslt = bmg250_set_fifo_wm(BMG250_FIFO_FRAME_FOR_WM_INT , &fifo_len, dev);
					/* FIFO watermark interrupt will be asserted henceforth
					 * when FIFO is filled until the set watermark level */
				if (rslt == BMG250_OK) {
					printf("\n Bytes to read as per watermark interrupt : %d ",fifo_len);
					dev->delay_ms(BMG250_FIFO_CONFIG_READ_DELAY);
					while (1) {
						/* Get interrupt status from the sensor */
						rslt = bmg250_get_int_status(&int_status,dev);
						printf("\n INT STATUS : %x ",int_status);
						if (int_status == FIFO_WM_INT_ASSERTED)
						{
							printf("\n FIFO watermark interrupt asserted");
						} else {
							printf("\n FIFO watermark interrupt not asserted");
						}
					}
				}
			}
		}
	}
	
	return rslt;
}
```
