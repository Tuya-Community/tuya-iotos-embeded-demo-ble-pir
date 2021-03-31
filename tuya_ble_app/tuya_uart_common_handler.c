
#include "tuya_ble_common.h"
#include "tuya_ble_mem.h"

#define DP_LEN_MAX       220
#define UART_HEAD_NUM    6
#define UART_FRAME_MAX  (220+4+7)


//MYFIFO_INIT(uart_rx_fifo, UART_FRAME_MAX+2, 4);
//MYFIFO_INIT(uart_tx_fifo, 255, 5);



void tuya_uart_common_send_bytes(u8* buf,u16 len)
{
	tuya_bsp_uart_send_bytes (buf, len);
}

u32 ty_uart_protocol_send(u8 type,u8 *pdata,u16 len)
{
    u8 alloc_buf[255+4+7];

    if(len+7>sizeof(alloc_buf)) return 1;
    //
    alloc_buf[0+1] = 0x55;
    alloc_buf[1+1] = 0xaa;
    alloc_buf[2+1] = 0x0;
    alloc_buf[3+1] = type;
    alloc_buf[4+1] = len>>8;
    alloc_buf[5+1] = len;

    memcpy(alloc_buf+6+1,pdata,len);
    alloc_buf[7+len] = check_sum(alloc_buf+1,6+len);
    tuya_uart_common_send_bytes(alloc_buf+1,7+len);
    return 0;
}
u32 ty_uart_debug_send(u8 type,u8 *pdata,u16 len)
{
    u8 alloc_buf[255+4+7];

    if(len+7>sizeof(alloc_buf)) return 1;
    //
    alloc_buf[0+1] = 0x77;
    alloc_buf[1+1] = 0xaa;
    alloc_buf[2+1] = 0x0;
    alloc_buf[3+1] = type;
    alloc_buf[4+1] = len>>8;
    alloc_buf[5+1] = len;

    memcpy(alloc_buf+6+1,pdata,len);
    alloc_buf[7+len] = check_sum(alloc_buf+1,6+len);
    tuya_uart_common_send_bytes(alloc_buf+1,7+len);
    return 0;
}

u32 ty_uart_protocol_factory_send(u8 type,u8 *pdata,u8 len)
{
    u8 alloc_buf[256];
    //
    alloc_buf[0+1] = 0x66;
    alloc_buf[1+1] = 0xaa;
    alloc_buf[2+1] = 0x0;
    alloc_buf[3+1] = type;
    alloc_buf[4+1] = 0;
    alloc_buf[5+1] = len;

    memcpy(alloc_buf+6+1,pdata,len);
    alloc_buf[7+len] = check_sum(alloc_buf+1,6+len);
    tuya_uart_common_send_bytes(alloc_buf+1,7+len);
    return 0;
}

s32 mcu_heartbeat_callback()
{
	return 0;
}
u32 ble_dpData_to_uart_dpData(u8* in_buffer,u16 in_len,u8* out_buffer,u16 out_buffer_len,u16*out_len)
{
	   u8 dp_len=0;
	   u16 offset=0;
	   u16 out_offset=0;

	   while((in_len-offset)!=0)
	   {
		   memcpy(out_buffer+out_offset,in_buffer+offset,2);
		   dp_len=in_buffer[offset+2];
		   out_buffer[out_offset+2]=0x00;
		   out_buffer[out_offset+3]=dp_len;
		   offset+=3;
		   out_offset+=4;
		   if((out_offset+dp_len)>out_buffer_len)
		   {
			   tuya_log_d("ble_dpData_to_uart_dpData too large");
			   return 1;
		   }
		   memcpy(out_buffer+out_offset,in_buffer+offset,dp_len);
		   out_offset+=dp_len;
		   offset+=dp_len;
		   if(in_len<offset)
		   {
			   tuya_log_d("ble_dpData_to_uart_dpData error");
			   return 2;
		   }
	   }
	   *out_len=out_offset;
	   return 0;
}

u32 uart_dpData_to_ble_dpData(u8* in_buffer,u16 in_len,u8* out_buffer,u16 out_buffer_len,u16*out_len)
{
	   u16 dp_len=0;
	   u16 offset=0;
	   u16 out_offset=0;

	   //tuya_log_dumpHex("uarx",50,in_buffer,10);
	   while((in_len-offset)!=0)
	   {
		   memcpy(out_buffer+out_offset,in_buffer+offset,2);
		   dp_len=(in_buffer[offset+2]<<8)+in_buffer[offset+3];
		   if(dp_len>255)
		   {
			   tuya_log_d("uart_dpData_to_ble_dpData dp too large-%d-%d-%x-%x",offset,dp_len,in_buffer[offset+2],in_buffer[offset+3]);
			   return 3;
		   }
		   out_buffer[out_offset+2]=dp_len;
		   offset+=4;
		   out_offset+=3;
		   if((out_offset+dp_len)>out_buffer_len)
		   {
			   tuya_log_d("uart_dpData_to_ble_dpData too large");
			   return 1;
		   }
		   memcpy(out_buffer+out_offset,in_buffer+offset,dp_len);
		   out_offset+=dp_len;
		   offset+=dp_len;
		   if(in_len<offset)
		   {
			   return 2;
		   }
	   }
	   *out_len=out_offset;
	   return 0;
}

u16 uart_rx_len=0;
static u8  uart_rx_buffer[UART_FRAME_MAX];
static u8 status =0;

s32 uart_timeout_handler(void)
{
	tuya_log_d("uart rx len-%d",uart_rx_len);
	uart_rx_len=0;
	tuya_log_dumpHex("uart rx",50,uart_rx_buffer,uart_rx_len>50?50:uart_rx_len);
    status=0;
    tuya_log_d("uart_timeout_handler");
    return -1;
}

u8 uart_data_unpack(u8 data)
{
	static u16 datalen =0;
	u8 err_code=1;
	u8 ck_sum;

    switch (status)
    {
    case 0:
        if(data==0x55||data==0x66||data==0x77)
        {
            uart_rx_buffer[status]=data;
            uart_rx_len=1;
            tuya_timer_start(TIMER_UART_RX_TIMEOUT,800);
            status=1;
        }
        break;
    case 1:
        if(data==0xAA)
        {
        	uart_rx_len++;
            uart_rx_buffer[status]=data;
            status=2;
        }
        else if(data==0x55||data==0x66||data==0x77)
        {
        	uart_rx_buffer[0]=data;
        	uart_rx_len=1;
        	status=1;
        }
        else
        {
			status=0;
        }
        break;
    case 2:
    	uart_rx_len++;
        uart_rx_buffer[status]=data;
        status=3;
        break;
    case 3:
    	uart_rx_len++;
        uart_rx_buffer[status]=data;
        uart_rx_len=4;
        status=4;
        break;
    case 4:
    	uart_rx_len++;
    	//data_len=data<<8;
        uart_rx_buffer[status]=data;
        status=5;
        break;
    case 5:
    	//data_len=data_len+data;
        uart_rx_buffer[status]=data;
        uart_rx_len=6;
        datalen=(uart_rx_buffer[4]<<8)+uart_rx_buffer[5];
        if(datalen==0)
            status=7;
        else if(datalen<=(UART_FRAME_MAX-7))
        {
        	//tuya_log_d("uart rx data len-%d",datalen);
            status=6;
        }
        else//长度超限制
        {
        	tuya_log_d("uart rx dp_len too large-%d-%d",datalen);
        	status=0;
        }
        break;
    case 6:
        if(uart_rx_len<=(UART_FRAME_MAX-2))
        {
            uart_rx_buffer[uart_rx_len++]=data;
        }
        else
        {
            uart_rx_len++;
        }
        if(uart_rx_len>=datalen+6)
            status=7;
        else
            status=6;
        break;
    case 7:
        tuya_timer_delete(TIMER_UART_RX_TIMEOUT);
        if(uart_rx_len<=(UART_FRAME_MAX-1))
        {
        	ck_sum = check_sum(uart_rx_buffer,uart_rx_len);
        	//tuya_log_d("uart rx crc-%d-%d-%d",ck_sum,data,uart_rx_len);
            uart_rx_buffer[uart_rx_len++]=data;
            if(ck_sum == data)
            {
            	err_code=0;
            }
            else
            {
            	err_code=2;
            }
        }
        //tuya_log_d("uart rx unpack-%d-%x-%x-%x-%x-%d",err_code,uart_rx_buffer[0],uart_rx_buffer[1],data,ck_sum,uart_rx_len);
        status=0;
        break;
    default:
        status=0;
        break;
    }
    //tuya_log_d("uart_rx frame-%d-0x%x",status,data);
    return err_code;
}
void tuya_uart_send_ble_dpdata(u8* ble_dp_data,u16 dp_len)
{
	u8 uart_data[220+4];//���е���dp�220
	u16 out_len=0;
	if(ble_dpData_to_uart_dpData(ble_dp_data,dp_len,uart_data,sizeof(uart_data),&out_len)==0)
	{
		ty_uart_protocol_send(TY_SEND_CMD_TYPE,uart_data,out_len);
	}
	else
	{
		tuya_log_d("send_ble_dpdata too large-%d",out_len);
	}
}
void tuya_uart_send_ble_state()
{
	ty_ble_state=tuya_ble_connect_status_get();
	//if(ty_ble_state>=UNBONDING_CONN) ty_ble_state=UNBONDING_UNCONN;
	if((ty_ble_state==UNBONDING_UNCONN)||(ty_ble_state==UNBONDING_CONN)||(ty_ble_state==UNBONDING_UNAUTH_CONN)||(ty_ble_state==UNKNOW_STATUS))
	{
		ty_ble_state=0;
		ty_uart_protocol_send(TY_REPORT_BT_STATE,&ty_ble_state,1);
	}
	else if(ty_ble_state==BONDING_UNCONN)
	{
		ty_ble_state=1;
		ty_uart_protocol_send(TY_REPORT_BT_STATE,&ty_ble_state,1);
	}
	else if(ty_ble_state==BONDING_CONN)
	{
		ty_ble_state=2;
		ty_uart_protocol_send(TY_REPORT_BT_STATE,&ty_ble_state,1);
	}
}
void tuya_uart_common_handler(u8 *pData,u16 len)
{
	u8 ble_buffer[220+3],err_code;
	u8 return_code=0;
	u16 da_len;
    u8 cmd=pData[3];

	u16 data_len=(pData[4]<<8)|(pData[5]<<0);
    u8* data_buffer=&pData[6];

    if(pData[2]!=0x00) return;//Э��汾�Ų���

    tuya_log_d("[uart_common]:cmd=0x%x,len=%d",pData[3],data_len);
	switch(cmd)
	{
		case TY_SEND_STATUS_TYPE:
			return_code=0;
			u16 out_len=0;
			u16 in_len=0;
			if(uart_to_ble_enable==0) return_code=3;
			if(!return_code)
			{//
				in_len=(pData[4]<<8)+pData[5];

				if((uart_dpData_to_ble_dpData(&pData[6],in_len,ble_buffer,sizeof(ble_buffer),&out_len))!=0)
				{
					return_code=6 ;
				}
				else if((err_code=tuya_ble_dp_data_report(ble_buffer,out_len))!=0)
				{
					return_code=0x10+err_code;
				}
			}
			ty_uart_protocol_send(TY_SEND_STATUS_TYPE,&return_code,1);
			break;
	}
}

void tuya_uart_debug_handler(u8 *pData,u16 len)
{
}

void tuya_uart_rx_handler(u8 *uart_Data,u16 len)
{
	u32 index=0;
	//tuya_log_d("tuya_uart_rx_handler-%d",len);

	if(tuya_get_ota_status() != TUYA_OTA_STATUS_NONE) return;//升级状态不处理串口数据

	while(index<len)
	{
		if(uart_data_unpack(uart_Data[index++])==0)
		{
			if(uart_rx_buffer[0]==0x55)
			{//正常指令集
				tuya_uart_common_handler(uart_rx_buffer,uart_rx_len);
			}
			else if((ty_factory_flag==1)&&(uart_rx_buffer[0]==0x66))
			{//生产指令集
				//tuya_log_v("ty_factory_flag:%d",ty_factory_flag);
				tuya_uart_factory_test(uart_rx_buffer,uart_rx_len);
			}
			else if(uart_rx_buffer[0]==0x77)
			{//调试指令集
				tuya_uart_debug_handler(uart_rx_buffer,uart_rx_len);
			}
		}
	}
}

void tuya_ble_custom_app_uart_common_process(uint8_t *p_in_data,uint16_t in_len)
{
}


