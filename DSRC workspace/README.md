This folder contains OMNeT++ simulation files necessary for running the MLR estimator performance evaluation simulations using the DSRC technology for V2I communication.

Requirements:
- Windows 10
- OMNeT++ 5.5.1
- INET 3.6.6
- Veins 4.7.1

4G-based C-V2X simulation network and configuration files are located in:
- [veins-veins-4.7.1/subprojects/veins_inet/examples/veins_inet/]([veins-veins-4.7.1/subprojects/veins_inet/examples/veins_inet/)

Please note that the folder with the OMNeT++ configuration (.ini) file, contains also pythonSimulationManager_v1.5.py file. This file is the Python implementation of the MLR Estimator which uses VISSIM and OMNeT++ output files from a shared directory to estimate the MLR. 
