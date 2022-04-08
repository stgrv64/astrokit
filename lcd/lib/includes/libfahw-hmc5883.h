#ifndef __FRIENDLYARM_HARDWARE_HMC5883_H__
#define __FRIENDLYARM_HARDWARE_HMC5883_H__

#define WriteAddress      (0x3C)
#define ReadAddress       (0x3D)
#define HMC5883_ADDRESS   (0x1E)
#ifndef PI
#define PI                (3.14159265)
#endif

double hmc5883Read(int devFD);
int hmc5883Init(int i2cDev);
void hmc5883DeInit(int devFD);

#endif
