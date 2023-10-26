# Changelog:
# 1.5: added averaging buffer to report maximum value of MLR over an averaging window

from genericpath import exists
import os.path
import time
import math
import numpy as np

#SIMULATION CONFIG
############################
averagingWindow = 15
evaluationTimeStart = 1
evaluationTimeEnd = 1500
receivedPath="tmpFiles/"
carsPath=""
MLRPath="/media/sf_OmnetExchange/MLR/"
pi=600
RSU_coord=[486.0,80.0]
simulationLength=evaluationTimeEnd
############################

simsecond = 0
FalseApproachVehicles=[]
reportingBuffer = [] #buffer where MLR is stored in order to report maximum value over the averagingWindow
LLV=[] #list of left vehicles
LPK_W=[] #list of potentially known vehicles
LPK_E=[] #list of potentially known vehicles
LPK_N=[] #list of potentially known vehicles
LPK_S=[] #list of potentially known vehicles

everCommunicated_W=[] #list of everCommunciated vehicles for NUNK algorithm
everCommunicated_E=[]
everCommunicated_N=[]
everCommunicated_S=[]

PCTraveltime_W = [] #travel time through P-C perimeter for each vehicle
PCTraveltime_E = []
PCTraveltime_N = []
PCTraveltime_S = []

NUNKW=0.0
NUNKE=0.0
NUNKN=0.0
NUNKS=0.0

def updateLLV(i):
    if i>1:
        if os.path.exists(carsPath+"{}.simsec".format(i)):
            carsi=open(carsPath+"{}.simsec".format(i),"r")
            carilines=carsi.readlines()
            carsi.close()
            cIids=[]
            for line in carilines:
                b=line.split(";")
                if b[1]!="9999":
                    cIids.append(int(b[1]))
            carsh=open(carsPath+"{}.simsec".format(i-1),"r")
            carhlines=carsh.readlines()
            carsh.close()
            cHids=[]
            for lin in carhlines:
                a=lin.split(";")
                if a[1]!="9999":
                    cHids.append(int(a[1]))
            for veh in cHids:
                if veh not in cIids:
                    LLV.append(veh)

def getVehicleApproach(vehID):
    if os.path.exists(carsPath+"{}.simsec".format(simsecond)):
        cars=open(carsPath+"{}.simsec".format(simsecond),"r")
        carlines=cars.readlines()
        cars.close()
        print("Searching approach for vehicle {}\n".format(vehID))
        for line in carlines:
            a=line.split(";")
            if int(a[1])==vehID:
                print("vehicle id should be {}\n".format(a[1]))
                if int(a[6])==8 or int(a[6])==2 or int(a[6])==10 or int(a[6])==14 or int(a[6])==12 or int(a[6])==13:
                    return "e"
                if int(a[6])==5 or int(a[6])==6 or int(a[6])==7:
                    return "n"
                if int(a[6])==3 or int(a[6])==4:
                    return "s"
                if int(a[6])==9 or int(a[6])==1 or int(a[6])==11:
                    return "w"
        else:
            print("vehicle {} not found in {}.simsec\n".format(vehID,simsecond))
            return "none"

def updateLPK(i):
    if os.path.exists(receivedPath+"{}.rcvd".format(i)):
        #time.sleep(0.2)
        ReceivedVehs=[] #list of received vehicles this second
        receivedf=open(receivedPath+"{}.rcvd".format(i),"r")
        rcvdlines=receivedf.readlines()
        receivedf.close()
        rcvdlines=rcvdlines[:-2]
        for line in rcvdlines:
            if line != "empty\n":
                ReceivedVehs.append(int(line))
                approach=getVehicleApproach(int(line))
                if approach=="w":
                    if int(line) not in LPK_W:
                        LPK_W.append(int(line))  
                if approach=="e":
                    if int(line) not in LPK_E:
                        LPK_E.append(int(line))  
                if approach=="n":
                    if int(line) not in LPK_N:
                        LPK_N.append(int(line))    
                if approach=="s":
                    if int(line) not in LPK_S:
                        LPK_S.append(int(line)) 
                if approach=="none":
                    if int(line) not in FalseApproachVehicles:
                        FalseApproachVehicles.append(int(line))
        for vehw in LPK_W:
            if vehw in LLV:
                LPK_W.remove(vehw)  
        for vehe in LPK_E:
            if vehe in LLV:
                LPK_E.remove(vehe)   
        for vehn in LPK_N:
            if vehn in LLV:
                LPK_N.remove(vehn) 
        for vehs in LPK_S:
            if vehs in LLV:
                LPK_S.remove(vehs) 
        print("Simsecond: {} Vehicles with undefined approach: {}\n".format(simsecond,FalseApproachVehicles))

def getReceived(i):
    if os.path.exists(receivedPath+"{}.rcvd".format(i)):
        #time.sleep(0.2)
        ReceivedVehs_W=[] #list of received vehicles this second
        ReceivedVehs_E=[] #list of received vehicles this second
        ReceivedVehs_N=[] #list of received vehicles this second
        ReceivedVehs_S=[] #list of received vehicles this second
        receivedf=open(receivedPath+"{}.rcvd".format(i),"r")
        rcvdlines=receivedf.readlines()
        receivedf.close()
        rcvdlines=rcvdlines[:-2]
        for line in rcvdlines:
            if line != "empty\n":
                approach=getVehicleApproach(int(line))
                if approach=="w":
                    ReceivedVehs_W.append(int(line))
                if approach=="e":
                    ReceivedVehs_E.append(int(line))
                if approach=="n":
                    ReceivedVehs_N.append(int(line))
                if approach=="s":
                    ReceivedVehs_S.append(int(line))
        return ReceivedVehs_W,ReceivedVehs_E,ReceivedVehs_N,ReceivedVehs_S

def addToAllCars(i):
    if os.path.exists(carsPath+"{}.simsec".format(simsecond)):
        cars=open(carsPath+"{}.simsec".format(simsecond),"r")
        carlines=cars.readlines()
        cars.close()
        allCars=open("allCarsPython.txt","a+")
        allCars.writelines(carlines)
        allCars.close()

def getVehicleCoordinates(vehID):
    if os.path.exists(carsPath+"{}.simsec".format(simsecond)):
        cars=open(carsPath+"{}.simsec".format(simsecond),"r")
        carlines=cars.readlines()
        cars.close()
        Coord=[]
        found=0
        for line in carlines:
            x=line.split(";")
            if int(x[1])==vehID:
                a=x[2].split()
                Coord.append(float(a[0]))
                Coord.append(float(a[1]))
                found=1
        if found==0:
            Coord.append("false")
            return Coord
        if found==1:
            return Coord

def getVehicleDistanceToRSU(Coord):
    distance=math.sqrt(((Coord[0]-RSU_coord[0])**2)+((Coord[1]-RSU_coord[1])**2))
    return distance

def getVehicleFirstTime(vehID):
    if os.path.exists(carsPath+"{}.simsec".format(simsecond)):
        cars=open(carsPath+"{}.simsec".format(simsecond),"r")
        carlines=cars.readlines()
        cars.close()
        firstTime=9999
        for line in carlines:
            x=line.split(";")
            if int(x[1])==vehID:
                firstTime=int(x[4])
        return firstTime

            

def updateNUNK(i):
    if os.path.exists(receivedPath+"{}.rcvd".format(i)):
        receivedf=open(receivedPath+"{}.rcvd".format(i),"r")
        rcvdlines=receivedf.readlines()
        receivedf.close()
        rcvdlines=rcvdlines[:-2]
        for vehID in rcvdlines:
            if vehID!="empty\n":
                Coord=getVehicleCoordinates(int(vehID))
                if Coord[0]!="false":
                    distance=getVehicleDistanceToRSU(Coord)
                    approach=getVehicleApproach(int(vehID))
                    firstTime=getVehicleFirstTime(int(vehID))
                    print("Vehicle {} distance: {}, approach: {}, firstTime: {}\n".format(vehID,distance,approach,firstTime))
                    if firstTime!=9999:
                        if approach == "w":
                            if int(vehID) not in everCommunicated_W:
                                everCommunicated_W.append(int(vehID))
                                travelTime=simsecond-firstTime
                                PCTraveltime_W.append(float(travelTime))
                        if approach == "e":
                            if int(vehID) not in everCommunicated_E:
                                everCommunicated_E.append(int(vehID))
                                travelTime=simsecond-firstTime
                                PCTraveltime_E.append(float(travelTime))
                        if approach == "n":
                            if int(vehID) not in everCommunicated_N:
                                everCommunicated_N.append(int(vehID))
                                travelTime=simsecond-firstTime
                                PCTraveltime_N.append(float(travelTime))
                        if approach == "s":
                            if int(vehID) not in everCommunicated_S:
                                everCommunicated_S.append(int(vehID))
                                travelTime=simsecond-firstTime
                                PCTraveltime_S.append(float(travelTime))

def calculateNUNK():
    F_W=float(len(everCommunicated_W))/float(pi)
    F_E=float(len(everCommunicated_E))/float(pi)
    F_N=float(len(everCommunicated_N))/float(pi)
    F_S=float(len(everCommunicated_S))/float(pi)

    time_W=np.array(PCTraveltime_W)
    meanTime_W=np.average(time_W)
    NUNK_W=meanTime_W*F_W

    time_E=np.array(PCTraveltime_E)
    meanTime_E=np.average(time_E)
    NUNK_E=meanTime_E*F_E

    time_N=np.array(PCTraveltime_N)
    meanTime_N=np.average(time_N)
    NUNK_N=meanTime_N*F_N

    time_S=np.array(PCTraveltime_S)
    meanTime_S=np.average(time_S)
    NUNK_S=meanTime_S*F_S

    f=open("outputPython.txt","a+")
    f.write("!!!CALCULATED NUNK!!!\nSimsecond: {}\nF_W:{}\nF_E:{}\nF_N:{}\nF_S:{}\nMean Times W,E,N,S:{}, {}, {}, {}\nNUNK W,E,N,S: {}, {}, {}, {}\n".format(simsecond,F_W,F_E,F_N,F_S,meanTime_W,meanTime_E,meanTime_N,meanTime_S,NUNK_W,NUNK_E,NUNK_N,NUNK_S))
    f.close()

    everCommunicated_W.clear()
    everCommunicated_E.clear()
    everCommunicated_N.clear()
    everCommunicated_S.clear()

    PCTraveltime_W.clear()
    PCTraveltime_E.clear()
    PCTraveltime_N.clear()
    PCTraveltime_S.clear()

    return NUNK_W, NUNK_E, NUNK_N, NUNK_S

def simulationStart(simulationLength):
    for i in range(evaluationTimeStart,simulationLength+1):
        while not os.path.exists(receivedPath+"{}.rcvd".format(i)):
            time.sleep(0.01)
        print(receivedPath+"{}.rcvd".format(i))
        if os.path.exists(receivedPath+"{}.rcvd".format(i)):
            time.sleep(0.01)
            global simsecond,NUNKW,NUNKE,NUNKN,NUNKS
            simsecond = i
            addToAllCars(simsecond)
            updateLLV(simsecond)
            updateLPK(simsecond) #time.sleep ENABLE!!!
            rcvd_W,rcvd_E,rcvd_N,rcvd_S=getReceived(simsecond) #time.sleep ENABLE!!!
            updateNUNK(simsecond)
            if simsecond==pi:
                NUNKW,NUNKE,NUNKN,NUNKS = calculateNUNK()
            if NUNKW != 0.0 or len(LPK_W) != 0:
                MLR_W=100*(1-((len(rcvd_W))/(len(LPK_W)+NUNKW)))
            else:
                MLR_W=0.0
            if NUNKE != 0.0 or len(LPK_E) != 0:
                MLR_E=100*(1-((len(rcvd_E))/(len(LPK_E)+NUNKE)))
            else:
                MLR_E=0.0
            if NUNKN != 0.0 or len(LPK_N) != 0:
                MLR_N=100*(1-((len(rcvd_N))/(len(LPK_N)+NUNKN)))
            else:
                MLR_N=0.0
            if NUNKS != 0.0 or len(LPK_S) != 0:
                MLR_S=100*(1-((len(rcvd_S))/(len(LPK_S)+NUNKS)))
            else:
                MLR_S=0.0
            f=open("outputPython.txt","a+")
            f.write("{};{}\n".format(simsecond,FalseApproachVehicles))
            f.close()
            
            if MLR_W < 0:
                MLR_W = 0
            if MLR_E < 0:
                MLR_E = 0
            if MLR_N < 0:
                MLR_N = 0
            if MLR_S < 0:
                MLR_S = 0
            
            outfile=open("EstMLRPython.csv","a+")
            outfile.write("{};{};{};{};{}\n".format(simsecond,MLR_W,MLR_E,MLR_N,MLR_S))
            outfile.close()
            
            avgArr = []
            if not np.isnan(MLR_W):
                avgArr.append(MLR_W)
            if not np.isnan(MLR_E):
                avgArr.append(MLR_E)
            if not np.isnan(MLR_N):
                avgArr.append(MLR_N)
            if not np.isnan(MLR_S):
                avgArr.append(MLR_S)
            npAvgArr = np.array(avgArr)
            avgMLR = np.mean(npAvgArr) #average MLR (approaches average)
            reportingBuffer.append(MLR_E)
            npReportingBuffer = np.array(reportingBuffer)
            MLR_E_max = npReportingBuffer.max()
            if len(reportingBuffer) >= averagingWindow:
                reportingBuffer.clear()
            MLRfile=open(MLRPath+"{}.txt".format(i),"w+")
            MLRfile.write("{}".format(MLR_E_max))
            #MLRfile.write("SimSecond;EST MLR_W;EST MLR_E;EST MLR_N;EST MLR_S\n{};{};{};{};{}\n".format(simsecond,MLR_W,MLR_E,MLR_N,MLR_S))
            MLRfile.close()
            #calculate MLR=100*(1-(len(rcvd)/(len(LPK)+NUNK))) - CHECK!!!
    print("Simulation finished successfully at simsecond: {}".format(simsecond))
outfile=open("EstMLRPython.csv","w+")
outfile.write("SimSecond;EST MLR_W;EST MLR_E;EST MLR_N;EST MLR_S\n")
outfile.close() 

simulationStart(simulationLength)
