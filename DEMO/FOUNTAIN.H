#define FntInnerPcls 1000
#define FntOuterPcls 2000
#define FntSpiralPclsOff (FntInnerPcls*3+FntOuterPcls)
#define FntSpiralPcls 500

#define FntSpawnOutPcl 400.0
#define FntSpawnInnerPcl 80.0

#define FntSpawnOutDisp 1.0
#define FntSpawnOutPow 30.0
#define FntSpawnOutPowTilt 5.0
#define FntSpawnOutPowRand 23.5
#define FntSpawnOutPowFreq 0.04

#define FntSpawnOutGrav 20.0

#define FntLightningTime1 500
#define FntLightningTime2 1500
#define FntLightningTime3 2200
#define FntLightningTime4 2700
#define FntLightningTime5 3200
#define FntLightningTime6 3500

#define FntLightningPcls  400

//#define FntPhotonStart 4500
//#define FntPhotonAtVortex 4650
#define FntVortexSwarm 4000
#define FntVortexMajorSwarm 4200

#define FntVortexStart 4400
#define FntVortexMax 4700


extern void Load_Vortex();
extern void Iterate_Photonbeam_Texture();
extern void Noise_Sample();
extern void Add_Vortex_ToScene();