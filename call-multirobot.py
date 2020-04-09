import os
import time
import subprocess

MAX_AGENTS = 100
MAX_SCEN = 26
MAX_FAILED = 5

code = "/Users/clarence/Desktop/try-multirobot" # path to code
cmap = "/Users/clarence/Desktop/multirobot/maps/warehouse-10-20-10-2-2.map" # path to map
scen = "/Users/clarence/Desktop/multirobot/maps/warehouse-scen-even/warehouse-10-20-10-2-2-even-" # template path to scenarios

answer = []
failed = [0] * MAX_SCEN

for agents in range(1, MAX_AGENTS):
    good = 0
    for num in range(1, MAX_SCEN):
        if (failed[num] <= MAX_FAILED):
            timeStarted = time.time()
            test = subprocess.Popen([code, cmap, scen + str(num) + ".scen", str(agents)])

            try:
                test.wait(timeout = 20)
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
