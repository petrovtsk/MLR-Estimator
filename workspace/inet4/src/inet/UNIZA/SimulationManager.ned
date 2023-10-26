package inet.UNIZA;

simple SimulationManager
{
    parameters:
        @display("i=block/network2");
        @class(SimulationManager);
      	double updateInterval=default(1); //how often the position should be updated in seconds
		string outputFileName;
		string vehicleNamesFile;
		string EVmoduleName;
		double startTime;
}

