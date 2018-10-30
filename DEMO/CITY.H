// definitions, constants, etc
//#define CITY_RAIN

const float rainMaxHeight = 2000.0f;
const float rainMaxDist = 3000.0f;

const Vector rainDir(2.0, -7.0, -1.0);
const float rainVelMult = 125.0;
const Vector rainVel(
	rainDir.x * rainVelMult,
	rainDir.y * rainVelMult,
	rainDir.z * rainVelMult
);

