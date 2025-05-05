#Necessary Packages
import random
import json
import uuid
import numpy as np
import matplotlib.pyplot as plt
import torch
import sklearn
import joblib
from Models import LSTMModel
from datetime import datetime, timedelta

# --- Robot Class ---
# Defines movement, scanning ability, and communication methods
class Robot:

    # Initial position of robot is either randomized or from center (depending on random_start value)
    #Movement is determined by movement range (10 means 5 in each cardinal direction)
    def __init__(self, robot_id, field_width, field_height, randomize, movement_range=10, scan_range=5, is_byzantine=False):
        self.id = robot_id
        self.is_byzantine = is_byzantine
        self.field_width = field_width
        self.field_height = field_height
        self.token_list = []
        self.movement_range = movement_range
        self.scan_range = scan_range
        if(randomize):
            self.position = np.random.rand(2) * [field_width, field_height]
        else:
            self.position = [field_width/2, field_height/2]
        self.found_tokens = []
        self.active = True

    def move(self):
        if not self.active:
            return
        move_vector = (np.random.rand(2) - 0.5) * self.movement_range
        self.position += move_vector
        self.position = np.clip(self.position, [0, 0], [self.field_width, self.field_height])

    def scan(self):
        if not self.active:
            return
        for token in self.token_list:
            if np.linalg.norm(self.position - token) < self.scan_range and token.tolist() not in self.found_tokens:
                self.found_tokens.append(token.tolist())

    #Communicates with the controller. All communications are logged. 
    def communicate(self, current_time):
        if not self.active:
            return [], None
        # Send faulty information if byzantine - fake tokens number +-3 from the actual value and position is random within the scan range
        if self.is_byzantine:
            fake_tokens = []
            num_fake = len(self.found_tokens) + random.randint(-3, 3)
            if(num_fake < 0):
                num_fake = 0
            for _ in range(num_fake):
                token_pos = self.position + ((np.random.rand(2) - 0.5) * self.scan_range * 2)
                fake_tokens.append(token_pos.tolist())
            log_entry = {
                "robot_id": self.id,
                "timestamp": current_time.isoformat(),
                "position": self.position.tolist(),
                "reported_tokens": fake_tokens,
                "num_reported_tokens": len(fake_tokens),
                "is_byzantine": 1
            }
            return fake_tokens, log_entry
        else:
            log_entry = {
                "robot_id": self.id,
                "timestamp": current_time.isoformat(),
                "position": self.position.tolist(),
                "reported_tokens": self.found_tokens,
                "num_reported_tokens": len(self.found_tokens),
                "is_byzantine": 0
            }
            return self.found_tokens, log_entry

# --- Controller Class---
# Communication center between robots. Runs consensus algoirthm on incoming signals and produces final results
class Controller:
    def __init__(self, robots, duration_seconds, comm_interval, consensus_algorithm, include_AI):
        self.robots = robots
        self.duration = duration_seconds
        self.comm_interval = comm_interval
        self.consensus_algorithm = consensus_algorithm
        self.communication_logs = []
        self.include_AI = include_AI
        self.model_type = None
        self.seq_len = None
        self.model = None

    def load_model(self, model_type, model_path, seq_len=0):
        self.model_type = model_type
        self.seq_len = seq_len
        if model_type == "random_forest":
            self.model = joblib.load(model_path)
        elif model_type == "lstm":
            self.model = LSTMModel(input_size=4, hidden_size=64, num_layers=1)
            self.model.load_state_dict(torch.load(model_path))
            self.model.eval()
        else:
            raise ValueError("Unsupported model type")

    def run(self):
        start_time = datetime.now()
        current_time = start_time

        # For when AI in included. recent_communications is specifically for LSTM
        recent_communications = {}
        detected_byzantines = {}

        while (current_time - start_time).total_seconds() < self.duration:
            for robot in self.robots:
                robot.move()
                robot.scan()
            if (current_time - start_time).total_seconds() % self.comm_interval == 0:
                reports = []
                features_batch = []
                robot_ids = []
                robots_sending = []
                for robot in self.robots:
                    report, log_entry = robot.communicate(current_time)
                    if log_entry:
                        if self.include_AI:
                            pos = np.array(log_entry["position"])
                            num_tokens = log_entry["num_reported_tokens"]
                            avg_distance = np.mean([
                                np.linalg.norm(pos - np.array(token))
                                for token in log_entry["reported_tokens"]
                            ]) if log_entry["reported_tokens"] else 0
                            feature = list(pos) + [num_tokens, avg_distance]

                            if self.model_type == "random_forest":
                                features_batch.append(feature)
                                robot_ids.append(robot.id)
                                robots_sending.append(robot)
                            elif self.model_type == "lstm":
                                if robot.id not in recent_communications:
                                    recent_communications[robot.id] = []
                                recent_communications[robot.id].append(feature)
                                if len(recent_communications[robot.id]) > self.seq_len:
                                    recent_communications[robot.id].pop(0)
                                if len(recent_communications[robot.id]) == self.seq_len:
                                    features_batch.append(np.array(recent_communications[robot.id]))
                                    robot_ids.append(robot.id)
                                    robots_sending.append(robot)
                        else:
                            reports.append(report)
                            self.communication_logs.append(log_entry)

                if self.include_AI and features_batch:
                    if self.model_type == "random_forest":
                        preds = self.model.predict(np.array(features_batch))
                    elif self.model_type == "lstm":
                        features_batch = torch.tensor(features_batch, dtype=torch.float32)
                        outputs = self.model(features_batch)
                        preds = torch.argmax(outputs, dim=1).numpy()

                    for robot_obj, robot_id, pred in zip(robots_sending, robot_ids, preds):
                        if pred == 1:  # Predicted as Byzantine
                            if robot_obj.active:
                                robot_obj.active = False
                                detected_byzantines[robot_id] = robot_obj.is_byzantine
                        reports.append(report)
                        self.communication_logs.append(log_entry)

            current_time += timedelta(seconds=1)

        active_robots = sum(robot.active for robot in self.robots)
        final_tokens = self.consensus_algorithm(reports, active_robots)

        if(self.include_AI):
            return final_tokens, detected_byzantines
        else:
            return final_tokens