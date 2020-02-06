#ifndef _MCP4725_H_
#define _MCP4725_H_

#define MCP4725_ADDRESS                 (0xC0)   // 1100000x - Assumes A0 is GND and A2,A1 are 0 (MCP4725A0T-E/CH)
#define MCP4725_READ                    (0x01)
#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC
#define MCP4726_CMD_WRITEDACEEPROM      (0x60)  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

int  mcp4725Init();
void mcp4725SetVoltage( uint16_t output, bool writeEEPROM );
void mcp472ReadConfig( uint8_t *status, uint16_t *value );

#endif

