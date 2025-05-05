import os
import json
import random
import shutil
import numpy as np
from datetime import datetime
from Classes import Controller, Robot

# --- Simulation setup ---
def setup_simulation(field_width, field_height, num_tokens, num_robots, randomize, movement_range, scan_range, percent_byzantine):
    tokens = [np.random.rand(2) * [field_width, field_height] for _ in range(num_tokens)]

    robots = []
    num_byzantine = int(num_robots * percent_byzantine)
    byzantine_indices = set(random.sample(range(num_robots), num_byzantine))

    for i in range(num_robots):
        robot_id = f"robot_{i}_{str(random.randint(1000,9999))}"
        robots.append(Robot(robot_id, field_width, field_height, randomize, movement_range, scan_range, is_byzantine=(i in byzantine_indices)))

    return tokens, robots

# --- Simple Consensus Algorithm ---
# Simplification is still valid as the threshold for consensus breakdown (3f+1; around 1/3) still holds
def pbft_majority_consensus(reports, active_robot_count):
    token_counter = {}
    for report in reports:
        for token in report:
            token_key = tuple(np.round(token, decimals=1))  # Round to avoid tiny float differences
            if token_key in token_counter:
                token_counter[token_key] += 1
            else:
                token_counter[token_key] = 1

    consensus_tokens = []
    majority_threshold = active_robot_count // 2  # More than half

    for token_key, count in token_counter.items():
        if count > majority_threshold:
            consensus_tokens.append(list(token_key))

    return consensus_tokens