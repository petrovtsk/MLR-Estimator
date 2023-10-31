This folder contains OMNeT++ simulation files necessary for running the MLR estimator performance evaluation simulations using the 4G-based and 5G-based C-V2X in infrastructural mode (using Uu interface).

Requirements:
- Linux environment (tested on Ubuntu 20.04.6 LTS)
- OMNeT++ 5.6.2
- INET 4.2.2
- Simu5G 1.1.0
- Veins 5.1
- Python 3.6

4G-based C-V2X simulation network and configuration files are located in:
Simu5G-1.1.0/simulations/LTEHaifa/

5G-based C-V2X simulation network and configuration files are located in:
Simu5G-1.1.0/simulations/NR/networks/
Simu5G-1.1.0/simulations/NR/standalone/

Please note that each folder containing OMNeT++ configuration (.ini) file, contains also pythonSimulationManager_v1.5.py file. This file is the Python implementation of the MLR Estimator which uses VISSIM and OMNeT++ output files from a shared directory to estimate the MLR. 
