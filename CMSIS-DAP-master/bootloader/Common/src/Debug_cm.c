#include <RTL.h>
#include "Debug_cm.h"
#include "swd_host.h"
#include "Flash.h"
#include "main.h"
#include "gpio.h"




uint8_t new_cmd_pending;

uint8_t write_buf[512] = {0,0,0,0};

uint32_t readreg = 0;
uint32_t writereg = 0;

void delay_us(uint32_t time);


extern OS_TID mainTask;

void t_changePC(unsigned long pcstr, unsigned long spstr)
{
	uint32_t sp = 0;
	uint32_t pc = 0;


	//PC
	t_halt();
	
	swd_read_core_register(13, &sp);
	swd_write_core_register(13, spstr);
	swd_read_core_register(13, &sp);
	swd_write_core_register(13, sp);
	swd_read_core_register(15, &pc);
	swd_write_core_register(15, pcstr);
	swd_read_core_register(15, &pc);
	swd_write_core_register(15, pc);

}

void t_ReadPC()
{
	uint32_t pc = 0;

	//PC
	t_halt();
	
	swd_read_core_register(15, &pc);
	swd_write_core_register(15, pc);

}



uint8_t t_halt()
{
     unsigned long i;   
     swd_write_long(Reg_CoreDebug_DHCSR_address, 0xA05F0003);
     
     for(i = 0; i < 10; i++)
     {
        if((swd_read_long(Reg_CoreDebug_DHCSR_address)&0x00020003) == 0x00020003)     // wait for core to halt
        {
           return 0; // success  -- core halted
        }
     }    
     return 1;     // timeout 
}


/******************************************************************************
*	t_go(void)
*
*	resume target execution from current PC
*
*	Return:	0 on success, non-zero on fail
*
******************************************************************************/
uint8_t t_go(void)
{
    swd_write_long(Reg_CoreDebug_DHCSR_address, 0xA05F0001);   
    return 0;
}



/*------------------------------------------------------------------------------
	t_read_mem
	------------
	Read data from the target memory

	type = type of memory
	addr = target address to read from
	width = size of the reads (8/16/32)
	count = total number of BYTES to read
	*data = pointer to data buffer to hold data that's read

	returns 0 on success, non-zero on fail
*/
int t_read_mem(unsigned char type, unsigned long addr, unsigned char width, int count, unsigned char *data)
{
    unsigned long current_buffer_address;
    unsigned long read_value;
    unsigned long num_bytes_already_read;
    unsigned long current_address_to_read;
    unsigned char read_type;
    
    if (count <= 0)
    {
      return osbdm_error_invalid_parameter; // 0 bytes to read
    }
     
    read_type = width / 8;
     
    if  (!((read_type==1)|(read_type==2)|(read_type==4)))
    {
      return osbdm_error_invalid_parameter; // not a supported type of read
    }   

    current_buffer_address = addr;
    current_address_to_read = addr;
    num_bytes_already_read = 0;

    while (num_bytes_already_read < count)
    {
        
        switch (read_type)
        {
        case 1 :
			 swd_read_byte(current_address_to_read,(uint8_t *)&read_value);   
             num_bytes_already_read = num_bytes_already_read + read_type;
             current_address_to_read = current_address_to_read + read_type;
       
                  
             *(data+current_buffer_address-addr) = read_value & 0x000000FF;

             current_buffer_address++;
        break;
        case 2 :
			 swd_read_word(current_address_to_read, (uint32_t *)&read_value);
             num_bytes_already_read = num_bytes_already_read + read_type;
             current_address_to_read = current_address_to_read + read_type;
       
             *(data+current_buffer_address-addr) = (read_value & 0x000000FF);
             current_buffer_address++;

             *(data+current_buffer_address-addr) = (read_value & 0x0000FF00) >> 8;
             current_buffer_address++;
        break;
        case 4 :
             read_value = swd_read_long(current_address_to_read);
             num_bytes_already_read = num_bytes_already_read + read_type;
             current_address_to_read = current_address_to_read + read_type;
       
       
             *(data+current_buffer_address-addr) = (read_value & 0x000000FF);
             current_buffer_address++;

             *(data+current_buffer_address-addr) = (read_value & 0x0000FF00) >> 8;
             current_buffer_address++;

             *(data+current_buffer_address-addr) = (read_value & 0x00FF0000) >> 16;
             current_buffer_address++;

             *(data+current_buffer_address-addr) = (read_value & 0xFF000000) >> 24;
             current_buffer_address++;
        break;
        } //endcase            
    }
    
    return osbdm_error_ok;  //success 
}

/*------------------------------------------------------------------------------
	t_write_mem
	----------------
	Write data to target memory

	type = type of memory
	addr = target address to write to
	width = size of the writes (8/16/32)
	count = total number of BYTES to be written
	*data = pointer to data buffer containing the data

	returns 0 on success
*/
int t_write_mem(unsigned char type, unsigned long addr, unsigned char width, int count, unsigned char *data)
{
     unsigned long current_buffer_address;
     unsigned long temp_long;
     unsigned long num_bytes_already_written;
     unsigned long temp_val;
     unsigned char write_type;
     
     if (count <= 0)
     {
         return osbdm_error_invalid_parameter;
     }
     
     write_type = width / 8;
     
     if  (!((write_type==1)|(write_type==2)|(write_type==4)))
     {
        return osbdm_error_invalid_parameter; // not a supported type of write
     }
     
     current_buffer_address = addr;
     num_bytes_already_written = 0;     
          
     while (num_bytes_already_written < count)
     {
         
        switch (write_type)
        {
            case 1 :
                 temp_val = 0x000000FF & *(data+current_buffer_address-addr);
                 temp_long = temp_val;
                 current_buffer_address++;
                 
                 swd_write_byte(current_buffer_address - 1, temp_long);

                 num_bytes_already_written = num_bytes_already_written + write_type;
                 
            break;
            case 2 :
                 temp_val = 0x000000FF & *(data+current_buffer_address-addr);
                 temp_long = temp_val;
                 current_buffer_address++;
                 temp_val = 0x000000FF & *(data+current_buffer_address-addr);
                 temp_long = temp_long | (temp_val << 8);
                 current_buffer_address++;
                 
                 swd_write_word(current_buffer_address - 2, temp_long);

                 num_bytes_already_written = num_bytes_already_written + write_type;
                 
            break;
            case 4 :
                 temp_val = 0x000000FF & *(data+current_buffer_address-addr);
                 temp_long = temp_val;
                 current_buffer_address++;
                 temp_val = 0x000000FF & *(data+current_buffer_address-addr);
                 temp_long = temp_long | (temp_val << 8);
                 current_buffer_address++;
                 temp_val = 0x000000FF & *(data+current_buffer_address-addr);
                 temp_long = temp_long | (temp_val << 16);
                 current_buffer_address++;
                 temp_val = 0x000000FF & *(data+current_buffer_address-addr);
                 temp_long = temp_long | (temp_val << 24);
                 current_buffer_address++;

                 swd_write_long(current_buffer_address - 4, temp_long);

                 num_bytes_already_written = num_bytes_already_written + write_type;
            break;
        } //endcase
     }
     
     return osbdm_error_ok;  //success 
}


void t_reset()
{
	TARGET_RESET_STATE test;

	test = RESET_PROGRAM;

	swd_set_target_state(test);

}

uint32_t filenb = 0;		//表示目标文件占用的扇区个数
uint32_t * pread = 0, *pwrite = 0;

void command_exec(void)
{
#if 0
  unsigned long address = 0x20002000;
	unsigned char type = 0;
	unsigned char width = 8;
	int count = 1;
	unsigned char data[10] = {0xaa,0x55,0xaa,0x78};


	
	if(new_cmd_pending == CMD_HALT)
	{
		t_halt();
		new_cmd_pending = 0;
	}
	else if(new_cmd_pending == CMD_GO)
	{
		t_go();
		new_cmd_pending = 0;
	}
	else if(new_cmd_pending == CMD_WRITE_MEM)
	{
		t_write_mem(type, address, width, count, data);
		new_cmd_pending = 0;
	}
	else if(new_cmd_pending == CMD_READ_MEM)
	{
		t_read_mem(type, address, width, count, data);
		new_cmd_pending = 0;

	}
	else if(new_cmd_pending == CMD_CORE_WRITE)
	{
		t_changePC(0,0);
		new_cmd_pending = 0;
	}
	else if(new_cmd_pending == CMD_CORE_RESET)
	{
		t_reset();
		new_cmd_pending = 0;
	}	
	else if(new_cmd_pending == CMD_CORE_READ)
	{
		t_ReadPC();
		new_cmd_pending = 0;
	}
	else
	{
		;
	}
#endif

#if 1

	uint32_t i = 0, j = 0, k = 0, l = 0;
	uint32_t read_count = 0;
	uint32_t retry = 0;
	BOOL led_state = __FALSE;
	uint32_t write_error = 0;
	uint32_t sectorsize = 0;
	uint32_t rambufsize = 0;
	uint32_t rambufaddr = 0;
	

	gpio_set_cdc_led(led_state);
  led_state = !led_state;


	//Start to program
	if (!gpio_get_pin_loader_state())
	{		
		//t_halt();
		gpio_set_cdc_led(1);
		while(!gpio_get_pin_loader_state())
		{
			;
		}
		gpio_set_cdc_led(0);
		t_reset();	

		
		
		for(i = 0; i < RETRY_TIME; i++)
		{
			write_error = 0;
			pwrite = (uint32_t *)KINETIS_FLASHPRG;
			pread = (uint32_t *)READBACK_ADDR;
			t_write_mem(0,FLASH_PC - 1, 8, 1024, (unsigned char *)pwrite);	
			t_read_mem(0,FLASH_PC - 1, 8, 4, (unsigned char *)pread);
		
			for(l = 0; l < 1; l++)
			{
				if(*pread != *pwrite)
				{
					write_error = 1;
				}
		
				*pread++;
				*pwrite++;
			}
		
			if(write_error == 0)
			{
				break;
			}
		
			
		}

		if(write_error == 1)
		{
			write_error = 0;
			return;
		}

		//return ;
		
			
		t_changePC(FLASH_PC,FLASH_SP);
		t_go();
		#if 1
		//判断Target是否运行
		while(1)
		{
			t_read_mem(0,HWESR, 32, 1, (unsigned char *)&readreg);
			
			if((readreg == CPU_WAIT))
			{
				retry = 0;
				read_count = 0;
				break;
			}
			else 
			{
				read_count++;
			}

			if(read_count > READ_TIME)
			{
				t_halt();
				t_write_mem(0,FLASH_PC - 1, 8, 1024, (unsigned char *)KINETIS_FLASHPRG);
				t_reset();
				t_changePC(FLASH_PC,FLASH_SP);
				t_go();
				read_count = 0;
				retry++;

			}
			//retry more times
			if(retry >= RETRY_TIME)
			{
				retry = 0;
				read_count = 0;
				break;
			}
		}
		#endif
		#if 1

		//计算烧写地址和大小

		t_read_mem(0,HWSID, 32, 1, (unsigned char *)&sectorsize);

		filenb = (*(uint32_t *)FILE_SIZE)/sectorsize;
		if((*(uint32_t *)FILE_SIZE)%sectorsize)
		{
			filenb++;
		}

		//开始擦除flash

		for(i = 0; i < filenb; i++)
		{
			gpio_set_cdc_led(led_state);
    		led_state = !led_state;
			//write byte len
			writereg = 0;
			t_write_mem(0,HWBCR, 32, 1, (unsigned char *)&writereg);
			//write addr
			writereg = 0x400*i;
			t_write_mem(0,HWBFR, 32, 1, (unsigned char *)&writereg);
		
			writereg = CMD_ERASE;
			t_write_mem(0,HWCMR, 32, 1, (unsigned char *)&writereg);
			
			writereg = CMD_RUN;
			t_write_mem(0,HWCSR, 32, 1, (unsigned char *)&writereg);
			//等待擦除完成
			while(1)
			{
				t_read_mem(0,HWCSR, 32, 1, (unsigned char *)&readreg);

				if((readreg == CMD_DONW))
				{
					read_count = 0;
					break;
				}
				else 
				{
					read_count++;
				}	

				if(read_count > READ_TIME)
				{
					read_count = 0;
					return;
				}
			}

		}
		#endif
		#if 1

		//开始编程
		//i = 0;
		//j = 0;

		t_read_mem(0,HWFCG1, 32, 1, (unsigned char *)&rambufsize);
		t_read_mem(0,HWFCG2, 32, 1, (unsigned char *)&rambufaddr);
		
		for(i = 0; i < filenb;i++)
		{
			for(j = 0; j < (sectorsize/rambufsize); j++)
			{

				gpio_set_cdc_led(led_state);
    			led_state = !led_state;
				//t_halt();
				//write buf addr
				writereg = rambufaddr;
				t_write_mem(0,HWBUFADDR, 32, 1, (unsigned char *)&writereg);	

		
				pwrite = (uint32_t *)(START_APP_ADDRESS + sectorsize*i + rambufsize*j);
				pread = (uint32_t *)READBACK_ADDR;
					t_write_mem(0,rambufaddr, 8, rambufsize, (unsigned char *)pwrite);
					
				//write word len
				writereg = rambufsize/4;			//编程一次写4个字节
				t_write_mem(0,HWBCR, 32, 1, (unsigned char *)&writereg);
				//write addr
				writereg = sectorsize*i + rambufsize*j;
				t_write_mem(0,HWBFR, 32, 1, (unsigned char *)&writereg);
				//write cmd
				writereg = CMD_PROGRA;
				t_write_mem(0,HWCMR, 32, 1, (unsigned char *)&writereg);
				//start to run
				writereg = CMD_RUN;
				t_write_mem(0,HWCSR, 32, 1, (unsigned char *)&writereg);	

				//t_go();

				//delay_us(10);
			
				//wait for down
				while(1)
				{
					t_read_mem(0,HWCSR, 32, 1, (unsigned char *)&readreg);
					if((readreg == CMD_DONW))
					{
						read_count = 0;
						break;
					}
					else 
					{
						read_count++;
					}	
				
					if(read_count > READ_TIME)
					{
						read_count = 0;
						return;
					}
					
				}			

			}

		}
		#endif

		isr_evt_set(PROGRAMMING_FINISH, mainTask);

	}
#endif	


}


void delay_us(uint32_t time)
{
	uint32_t i,j;

	for(i = 0; i < time ;i++)
	{
		for(j = 0; j < 100; j++)
		{
			;
		}
	}


}
















