class VehPos {
  public:
    inet::Coord position;
    double distanceToRSU;
    int vehID;
    int omnetID;
    bool enteredSimulation;
    bool leftSimulation;
    bool assignedDirection;
    int firstDetectionSecond;
    int firstCommunicatedSecond;
    const char *direction;
    void set_Position(const inet::Coord& pos) {position = pos;}
    bool approaching;
    double startTime;
};
