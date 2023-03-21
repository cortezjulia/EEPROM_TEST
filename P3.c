#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/eeprom.h"
#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

int saida=0, end;
uint32_t num[512],total[512],teste[512],soma;
uint32_t maior,menor=0;
uint32_t inicial,final,intervalo=0;
char x;
int i,q;
void Config_UART0(void);
uint32_t UARTDecGet(uint32_t);

void main(void)
{
  Config_UART0();
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
  
  while(saida!='s')
  {
    UARTprintf("<<<<<<<<<<<<<< Memória EEPROM >>>>>>>>>>>>>>>>> \n");
    if (EEPROMInit()==EEPROM_INIT_OK) UARTprintf("\nStatus da EEPROM: OK");
    else if (EEPROMInit()==EEPROM_INIT_ERROR) UARTprintf("\nStatus da EEPROM: ERRO");
    
    UARTprintf("\x1B[2J\x1B[0;0H");
    UARTprintf("\nEscolha a tarefa desejada:\n");
    UARTprintf("\n        1->Ler a EEPROM");
    UARTprintf("\n        2->Gravar na EEPROM");
    UARTprintf("\n        3->Ver soma, maior e menor número da EEPROM");
    UARTprintf("\n        4->Apagar parcialmente a  EEPROM");
    UARTprintf("\n        5->Apagar totalmente a EEPROM");
    
    x=UARTCharGet(UART0_BASE);
    
    switch (x)
    {
    case '1':
      UARTprintf("\x1B[2J\x1B[0;0H");
      UARTprintf("\n\nLendo a EEPROM...\n END");
      EEPROMRead(total, 0, sizeof(total));
      
      for (i=0; i<=511; i++)
      {
        if (i%10==0) UARTprintf("\n %3d ", i);
        UARTprintf("%8d ", total[i]);
      }
      break;
      
    case '2':
      UARTprintf("\x1B[2J\x1B[0;0H");
      UARTprintf("\nDigite a quantidade de números desejados:");
      
      q=UARTDecGet(UART0_BASE);
      
      while(q<0 || q>511)
      { 
        UARTprintf("\nVocê ultrapassou o limite!");
        UARTprintf("\nDigite a quantidade de números desejados:");
        q=UARTDecGet(UART0_BASE);
      }
      UARTprintf("\n");
      for(int i=0; i<q;i++)
      {
        UARTprintf("[%d]-",i+1);
        num[i]=UARTDecGet(UART0_BASE);
        UARTprintf("   ");
      }  
      UARTprintf("\nDigite o endereço inicial de gravação: ");
      end=UARTDecGet(UART0_BASE);
      
      EEPROMProgram(num,end*4,(q*4));
      UARTprintf("\n\nVocê escolheu %d valores...", q);
      
      break;
      
    case '3':
      menor=num[0];
      UARTprintf("\x1B[2J\x1B[0;0H");
      EEPROMRead(total, 0, sizeof(total));
      for (i=0; i<=511; i++)
      {
        if(total[i]!=0xFFFFFFFF)
        {
          soma=total[i]+soma;
          
          if(total[i]>maior)
          {
            maior=total[i];
          }
          if(total[i]<menor)
          {
            menor=total[i];
          }
        }
        
      }
      UARTprintf("\nA soma dos elementos da EEPROM é:%d",soma);
      UARTprintf("\nO maior dos elementos da EEPROM é:%d",maior);
      UARTprintf("\nO menor dos elementos da EEPROM é:%d",menor);
      soma=0;
      maior=0;
      menor=0xFFFFFFFF;
      break;
      
    case '4':
      
      UARTprintf("\x1B[2J\x1B[0;0H");
      UARTprintf("\nInsira o endereço INICIAL\n"); 
      UARTprintf("do intervalo que deseja apagar: ");
      inicial=UARTDecGet(UART0_BASE);
      while(inicial<0 || inicial>511)
      { 
        UARTprintf("\nVocê ultrapassou o limite!");
        UARTprintf("\nInsira o endereço INICIAL\n"); 
        UARTprintf("do intervalo que deseja apagar: ");
        inicial=UARTDecGet(UART0_BASE);
      }
      UARTprintf("\nInsira o endereço FINAL\n"); 
      UARTprintf("do intervalo que deseja apagar: ");
      final=UARTDecGet(UART0_BASE);
      while(final<0 || final>511 || final<inicial)
      { 
        UARTprintf("\nVocê ultrapassou o limite!");
        UARTprintf("\nInsira o endereço FINAL\n"); 
        UARTprintf("do intervalo que deseja apagar: ");
        final=UARTDecGet(UART0_BASE);
      }
      intervalo=(final-inicial);
      if(intervalo>0)
      {
        for(i=0; i <= intervalo; i++)
        {
          teste[i]=0xFFFFFFFF;
        }
        EEPROMProgram(teste, inicial*4, ((intervalo+1)*4));
      }
      UARTprintf("\n\nO intervalo [%d,%d] acabou de ser apagado!", inicial, final);
      
      break;
      
    case '5':
      
      
      UARTprintf("\x1B[2J\x1B[0;0H");
      UARTprintf("\nMemória acabou de ser apagada...");
      EEPROMMassErase();
      break;
    }
    UARTprintf("\nAperte s p/ sair ou qualquer tecla p/ continuar");
    saida=UARTCharGet(UART0_BASE);
  }
  while(saida=='s')
  {
    UARTprintf("\x1B[2J\x1B[0;0H");
    UARTprintf("\r****SISTEMA ENCERRADO****");
  }
}
void Config_UART0(void)
{
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);
  UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
  UARTStdioConfig(0, 115200, 16000000);
}
uint32_t UARTDecGet(uint32_t ui32Base)
{
  uint32_t numero=0,tamanho=0;
  char caractere;
  
  caractere= UARTCharGet(ui32Base);
  while(caractere!= 0x0D)//CR - ENTER
  {
    if((caractere>='0')&&(caractere<='9'))
    {
      numero=10*numero+(caractere-0x30);
      tamanho++;
      UARTCharPut(ui32Base, caractere);
    }
    else if((caractere==0x08)&&(tamanho!=0))//backspace
    {
      numero/=10;
      tamanho--;
      UARTCharPut(ui32Base, caractere);
    }
    caractere= UARTCharGet(ui32Base);
  }
  return numero;
}


