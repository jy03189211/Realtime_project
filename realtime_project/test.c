#include "MultiSensorSimulator.h"
#include <stdio.h>
int main(void)
{
	int noOfSensors;
	Tmeas measurement;
	int sensorDescriptors [20];
	noOfSensors = StartSimulator(sensorDescriptors, 5);
	read(sensorDescriptors[0], &measurement, sizeof(Tmeas));
	printf(“Measurement value was %d\n”, measurement.value);
	return 0;
}