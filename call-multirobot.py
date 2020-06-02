import os
import time
import subprocess

MAX_AGENTS = 100
MAX_SCEN = 26
MAX_FAILED = 4
TIME_OUT = 30

#code = "/Users/clarence/Desktop/multirobot/code/try-multirobot" # path to code
#cmap = "/Users/clarence/Desktop/multirobot/maps/brc202d.map" # path to map
#scen = "/Users/clarence/Desktop/multirobot/maps/brc202d-scen-even/brc202d-even-" # template path to scenarios

dijkstra_precalc = "true"
use_CAT = "true"
heuristic = "normal" # (normal, number_of_conflicts, number_of_conflicting_agents, number_of_pairs, vertex_cover)
prioritize_conflicts = "false"
use_bypass = "false"
use_ecbs = "false"
omega = "1.1"

#code = "/Users/clarence/Desktop/try-multirobot"
#cmap = "/Users/clarence/Desktop/multirobot/maps/room-64-64-8.map"
#scen = "/Users/clarence/Desktop/multirobot/maps/room-scen-even/room-64-64-8-even-"

#code = "/Users/clarence/Desktop/try-multirobot"
#cmap = "/Users/clarence/Desktop/multirobot/maps/warehouse-10-20-10-2-2.map"
#scen = "/Users/clarence/Desktop/multirobot/maps/warehouse-scen-even/warehouse-10-20-10-2-2-even-"

#code = "/Users/clarence/Desktop/try-multirobot"
#cmap = "/Users/clarence/Desktop/multirobot/maps/empty-16-16.map"
#scen = "/Users/clarence/Desktop/multirobot/maps/empty-16-16-scen-even/empty-16-16-even-"

#code = "/Users/clarence/Desktop/try-multirobot"
#cmap = "/Users/clarence/Desktop/multirobot/maps/empty-8-8.map"
#scen = "/Users/clarence/Desktop/multirobot/maps/empty-8-8-scen-even/empty-8-8-even-"

answer = []
failed = [0] * MAX_SCEN

for agents in range(1, MAX_AGENTS):
    good = 0
    for num in range(1, MAX_SCEN):
        if (failed[num] <= MAX_FAILED):
            timeStarted = time.time()
            test = subprocess.Popen([code, cmap, scen + str(num) + ".scen", str(agents), dijkstra_precalc, use_CAT, heuristic, prioritize_conflicts, use_bypass, use_ecbs, omega])

            try:
                test.wait(timeout = TIME_OUT)
            except subprocess.TimeoutExpired:
                test.kill()
                failed[num] += 1
                print("ag:", agents, "test:", num, "res: BAD")
            else:
                good += 1
                print("ag:", agents, "test:", num, "res: GOOD")

            timeDelta = time.time() - timeStarted
            print("%.5f" % timeDelta)

        else:
            print("ag:", agents, "test:", num, "res: BAD")

    answer.append(good)
    print(answer)
