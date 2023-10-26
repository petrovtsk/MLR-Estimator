package inet.UNIZA;

simple PacketLossEvaluator
{
    parameters:
        @display("i=block/network2");
        @class(PacketLossEvaluator);
      	double updateInterval=default(1); //how often the position should be updated in seconds
		string outputFileName;
		//string vehicleNamesFile;
		//string EVmoduleName;
		//double startTime;
}

