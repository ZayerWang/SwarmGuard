# SwarmGuard - Using AI to Enhance Consensus Algorithms in Robotic Swarm Communications
Isaiah Wang - CSC 516: Cybersecurity

# Background
This project is a proof-of-concept for using Machine Learning techniques and AI to enhance consensus in a swarm network. As robotics continues to develop, a growing field of interest is in the application of swarms of smaller well connected robots. Secure and reliable coordination among multiple autonomous robots is critical in swarm robotics applications such as environmental monitoring, search-and-rescue, and underwater surveying. However, when some robots become compromised—whether due to hardware failures, software bugs, or malicious cyberattacks—they can behave arbitrarily (Byzantine faults), potentially derailing the mission. This project develops an end-to-end pipeline: from simulating Byzantine-fault-tolerant swarm behaviors, to generating labeled datasets, to training machine-learning models that detect compromised agents in real time. By combining realistic robot-motion simulations with rigorous AI methods, we aim to enhance swarm resilience and provide operators with timely alerts when a subset of robots deviates from expected cooperative behavior.

# Data
Since public data for swarm robotics communication is rare, this project generates its own data. The data is simplified from real systems but aims to emulate realistic information that could be gathered in real swaram robotic scenarios. A simulation was created in which a field of variable size is initialized. Within the field exists tokens and robots, and a controller that oversees the simulation. The robots are given a set amount of time, and every time step can randomly move and scan their surroundings. The goal for the swarm is to arrive at a consensus on the number of tokens in the field and where those tokens are. Every timestep, the simulator logs each robot’s position, heading, token-pickup events, communication messages (if any), and a ground-truth “compromised” flag. Instead of one file per robot, all entries are written to a single json file. By varying parameters—arena size, token density, Byzantine activation rate, and movement noise one can generate much data that capture both normal and adversarial swarm behaviors.
**Note**: Because of the simulation’s design, adjusting environment variables (e.g., num_robots, run_time) requires  tuning to find the right balance. For example, over a long enough run,even with 20–40% Byzantine robots, the honest agents will repeatedly encounter the correct tokens, causing the consensus algorithm to overwhelmingly favor matching tokens and yield high‐accuracy results. With the current configurations, consensus remains strong when Byzantine participation is below 33%, but accuracy drops off sharply once that threshold is exceeded.

# Model Training
Currently, there are two models available to test: A classical classifier (Random Forest) and a sqeunce model (LSTM). Random Forests was chosen for its ability handling heterogeneous & tabular feature sets—such as summary statistics of movement smoothness, inter-robot distances, and token-pickup irregularities—without requiring extensive feature engineering or careful scaling.LSTM was chosen as memory cells could capture temporal dependencies in raw time-series data, enabling detection of subtle or coordinated anomalies over many timesteps, such as in this scenario where logs are coming in every set timestep. 
**Note**: In addition to these models strengths, hardware and resource limitations also were also considered. Thus smaller models that could train quickly, even on CPU or a laptop, were chosen more preferably 

# ARGoS
Plugin files for an ARGoS (https://www.argos-sim.info/) based simulation are included as well. Currently, this version of the simulation is in development and has not fully developed as a result of dependency issues. The goal is to do the same simulation as what the Python code currently does, but more sophisticated and in a system that could be uploaded to an actual swarm.  

# Looking Forward
This is an ongoing project that I am planning to continue working on and adding to. 
Possible updates include:
- More communication network
- More complex environment (obstacles in field, elevation differences, different tokens)
- "Smarter" byzantines (all byzantines working together to deceive for example)

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
