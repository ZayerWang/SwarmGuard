# SwarmGuard - Using Artificial Intelligence to Enhance Consensus Algorithms in Robotic Swarm Communications
Isaiah Wang - CSC 516: Cybersecurity

# Overview
## Project Background
This project is a proof-of-concept for using Machine Learning (ML) techniques and Artificial Intelligence (AI) to enhance consensus in a swarm network. As robotics continues to develop, a growing field of interest is in the application of swarms of smaller well connected robots. Secure and reliable coordination among multiple autonomous robots is critical in swarm robotics applications such as environmental monitoring, search-and-rescue missions, and self-driving cars. However, when some robots become compromised—whether due to hardware failures, software bugs, or malicious cyberattacks—they can behave undesirably (Byzantine faults), potentially hindering the mission. 

Current solutions to these issues involve the use of consensus algorithms to make swarm wide decisions. However, modern consensus algorithms often are unable to detect and remove intruders, thus leaving some systems vulnerable to attacks. The use of AI to improve consensus algorithms in byzantine systems is therefore desireable, especially if the AI can detect and remove the byzantine individuals. 

This project develops an end-to-end pipeline: from simulating Byzantine-fault-tolerant swarm behaviors, to generating labeled datasets, to training machine-learning models that detect byzantine individuals in real time. By combining swarm simulations with ML & AI methods, this project shows a novel way to enhance swarm security and resilience. While the exact implementation shown cannot be directly ported into a physical system, the project does shohw promise in the use of AI to improve cybersecurity in the field swarm robotics.

## Data
Since public data for swarm robotics communication is rare, this project generates its own data. The data is simplified from real systems but aims to emulate realistic information that could be gathered in real swaram robotic scenarios. A simulation was created in which a field of variable size is initialized. Within the field exists tokens and robots, and a controller that oversees the simulation. The robots are given a set amount of time, and every time step can randomly move and scan their surroundings. The goal for the swarm is to arrive at a consensus on the number of tokens in the field and where those tokens are. 

Every timestep, the simulator logs each robot’s communication message which includes the robot's ID, position, identified tokens, and a ground-truth “compromised” flag. Instead of one file per robot, all entries are written to a single json file. By varying parameters—arena size, token density, Byzantine activation rate, and movement options-one can generate much data that captures both normal and adversarial swarm behaviors.

**Note**: Because of the simulation’s design, adjusting environment variables (e.g., num_robots, run_time) requires  tuning to find the right balance. For example, over a long enough run,even with 20–40% Byzantine robots, the honest agents will repeatedly encounter the correct tokens, causing the consensus algorithm to overwhelmingly favor matching tokens and yield high‐accuracy results. With the current configurations, consensus remains strong when Byzantine participation is below 33%, but accuracy drops off sharply once that threshold is exceeded.

## Model Training
Currently, there are two models available to test: A classical classifier (Random Forest) and a sqeunce model (LSTM). Random Forests was chosen for its ability handling heterogeneous & tabular feature sets—such as summary statistics of movement smoothness, inter-robot distances, and token-pickup irregularities—without requiring extensive feature engineering or careful scaling.LSTM was chosen as memory cells could capture temporal dependencies in raw time-series data, enabling detection of subtle or coordinated anomalies over many timesteps, such as in this scenario where logs are coming in every set timestep. 

**Note**: In addition to these models strengths, hardware and resource limitations also were also considered. Thus smaller models that could train quickly, even on CPU or a laptop, were chosen more preferably 

# Codebase
## Dependencies
All required dependencies can be found in "SwarmGuard.yml"
The following code will create and activate and environment will all required dependencies.
```bash
conda env create -f SwarmGuard.yml
conda activate SwarmGuard
```

## Workflow
There are three main stages to this project. Data generation, model training, and testing with the trained model. The jupyter notebooks follow this
> GenerateData.ipynb -> LSTM/RandomForest.ipynb -> Test.ipynb

GenerateData.ipynb will output set number of json log files into a directory named "training_logs". Number of runs and directory name can be changed.

Training notebooks will load data from "training_logs" and train the corresponding model. The trained model will then be saved in a new directory called "trained_models"

Test.ipynb will load the selected model from "trained_models" and test the AI on a new simulation. Visualization of the simulation results will also be shown.  

## Code Structure
- Classes.py contains the Robot and Controller. The controller controls the simulation output, communication between robots, consensus calculations, and AI inputs (if applicable). Any modifications to the communication network, major swarm dynamics, or AI functionality should be made here.
- Utils.py contains extra functions and tools used throughout the project (eg. consensus algorithms & simulation set up). Modifications to the simulation environment or consensus method, and implementation of any additional functionality should be made here.
- Models.py contains the ML models used. Additionally, any new models added should include a data loader specific for that model (or specify an existing data loader that is sufficient). 
- Jupyter Notebooks: Follow workflow in order to run, train, and test the model correctly.

# ARGoS Simulator
Plugin files for an ARGoS (https://www.argos-sim.info/) based simulation are included in this repository. The original vision for this project involved using ARGoS as the simulation basis as it is known to be an applicable representation of swarms (Strobel et al., 2020). However, this version of the simulation is currently in development and has not fully tested as a result of dependency issues. The goal is to eventually have a simulation that could be tested virtually but also uploaded into an existing swarm. 


# Future Considerations
This is an ongoing project that I plan to continue working on and add to. 
Possible updates include:
- More complex environment (obstacles in field, different token types, etc.)
- "Smarter" byzantines (all byzantines working together to deceive, ie. "ghost tokens")
- Improved swarms (different robot types, adaptive exploration algorithm, more complex communication network etc.)
- Larger selection of consensus algorithm bases
- Testing more classical ML classifiers

Additionally, the idea of using AI to enhance consensus in swarms and networks is a developing field. Future considerations for the entire field could include:
- Testing on a physical swarm (test responsiveness of AI and plausible damage by a cyber attack).
- A combination structure that uses or combines traditional consensus algorithms, AI, and blockchain technology to enhance security and consensus.
- Larger and more complex ML models such as GNNs, adversarial models, or federated learning models to increase accuracy and security.

# References
- Amjadi, A. S., Bilaloğlu, C., Turgut, A. E., Na, S., Şahin, E., Krajník, T., & Arvin, F. (2023). Reinforcement learning-based aggregation for robot swarms. Adaptive Behavior, 32(3), 265–281. https://doi.org/10.1177/10597123231202593
- Botta, A., Rotbei, S., Zinno, S., & Ventre, G. (2023). Cybersecurity of robots: A comprehensive survey. Intelligent Systems With Applications, 18, 200237. https://doi.org/10.1016/j.iswa.2023.200237
- Chen, P., Han, D., Weng, T., Li, K., & Castiglione, A. (2021). A novel Byzantine fault tolerance consensus for Green IoT with intelligence based on reinforcement. Journal of Information Security and Applications, 59, 102821. https://doi.org/10.1016/j.jisa.2021.102821
- Dias, P. G. F., Silva, M. C., Filho, G. P. R., Vargas, P. A., Cota, L. P., & Pessin, G. (2021). Swarm Robotics: A perspective on the latest reviewed concepts and applications. Sensors, 21(6), 2062. https://doi.org/10.3390/s21062062
- FengYing, Y., Din, A., HuiChao, L., Babar, M., & Ahmad, S. (2024). Decentralized consensus in robotic swarm for collective collision and avoidance. IEEE Access, 12, 72143–72154. https://doi.org/10.1109/access.2024.3402564
- Gutiérrez, Á. (2022). Recent advances in swarm Robotics Coordination: communication and memory challenges. Applied Sciences, 12(21), 11116. https://doi.org/10.3390/app122111116
- Krishnamohan, T. (2023). A review of the consensus achievement strategies in the context of swarm robotics. Modern Intelligent Times. https://doi.org/10.53964/mit.2023001
- Kuzlu, M., Fair, C., & Guler, O. (2021). Role of Artificial Intelligence in the Internet of Things (IoT) cybersecurity. Discover Internet of Things, 1(1). https://doi.org/10.1007/s43926-020-00001-4
- Lamperti, R. D., & De Arruda, L. V. R. (2023). Distributed strategy for communication between multiple robots during formation navigation task. Robotics and Autonomous Systems, 169, 104509. https://doi.org/10.1016/j.robot.2023.104509
- Lu, Y., & Da Xu, L. (2018a). Internet of Things (IoT) Cybersecurity Research: A review of current research topics. IEEE Internet of Things Journal, 6(2), 2103–2115. https://doi.org/10.1109/jiot.2018.2869847
- Lu, Y., & Da Xu, L. (2018b). Internet of Things (IoT) Cybersecurity Research: A review of current research topics. IEEE Internet of Things Journal, 6(2), 2103–2115. https://doi.org/10.1109/jiot.2018.2869847
- Strobel, V. C. F. E. D. M. (2018, May 25). Managing Byzantine robots via blockchain technology in a swarm robotics collective decision making scenario. http://hdl.handle.net/1721.1/115883
- Strobel, V., Ferrer, E. C., & Dorigo, M. (2020). Blockchain technology secures robot swarms: a comparison of consensus protocols and their resilience to Byzantine robots. Frontiers in Robotics and AI, 7. https://doi.org/10.3389/frobt.2020.00054
- Zhao, H., Pacheco, A., Strobel, V., Reina, A., Liu, X., Dudek, G., & Dorigo, M. (2023). A generic framework for Byzantine-Tolerant consensus achievement in robot swarms. 2021 IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS), 8839–8846. https://doi.org/10.1109/iros55552.2023.10341423
