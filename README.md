# CBS-2020

Программу можно запустить двумя способами: вручную или с помощью скрипта call-multirobot.py. В скрипте call-multirobot.py можно настроить следующие параметры:

path_to_bin = "try-multirobot" # путь к исполняемому файлу<br>
path_to_map = "brc202d.map" # путь к файлу с картой<br>
paths_to_scen = ["brc202d-even-1.scen",<br>
                 "brc202d-even-2.scen",<br>
                 "brc202d-even-3.scen"] # набор путей к файлам со сценариями<br>

MAX_AGENTS = 50 # ограничение на количество агентов<br>
MAX_SCEN = len(paths_to_scen) # ограничение на номер сценария<br>
MAX_FAILED = 4 # ограничение на количество нерешенных задач<br>
TIME_OUT = 30 # ограничение по времени<br>

dijkstra_precalc = "true" # true -> использовать преподсчет с помощью алгоритма Dijkstra<br>
use_CAT = "true" # true -> использовать Conflict Avoidance Table<br>
heuristic = "normal" # эвристика (normal, number_of_conflicts, number_of_conflicting_agents, number_of_pairs, vertex_cover)<br>
prioritize_conflicts = "false" # true -> использовать приоритизацию конфликтов<br>
use_bypass = "false" # true -> использовать bypass<br>
use_ecbs = "false" # true -> использовать ecbs<br>
omega = "1.1" # коэффициент субоптимальности<br>
print_paths = "true" # true -> выводятся пути для каждого агента, false -> выводится только success rate<br>

Скрипт работает так: для каждого количества агентов от левой границы до MAX_AGENTS, для каждого сценария из paths_to_scen от левой границы до MAX_SCEN, запустить исполняемый файл path_to_bin с данными параметрами. Если, например, текущее количество агентов равно 5, из каждого сценария выбираются первые 5 агентов. Success rate вычисляется как процент выполненных за TIME_OUT сценариев.

Скрипт выдает следующую информацию:

agents: 1 scenario: 0 time: 0.07509 result: SUCCESS<br>
agents: 1 scenario: 1 time: 0.03795 result: SUCCESS<br>
agents: 1 scenario: 2 time: 0.04000 result: SUCCESS<br>

success rates for each number of agents: [100.0]<br>

Количество агентов, номер сценария, время, результат работы - SUCCESS или FAILURE. После выполнения всех сценариев для текущего количетсва агентов выводится success rate.

Можно запустить исполняемый файл вручную с параметрами path_to_bin, path_to_map, path_to_scen, num_agents, dijkstra_precalc, use_CAT, heuristic, prioritize_conflicts, use_bypass, use_ecbs, omega, print_paths.

Формат карты и файла со сценарием совпадает с форматом карт из этого репозитория https://movingai.com/benchmarks/mapf/index.html. Простая карта выглядит следующим образом:

type octile<br>
height 3<br>
width 3<br>
map<br>
...<br>
...<br>
...<br>

На второй строчке указана высота карты, на третьей ширина карты, далее идет матрица из символов. Символы . и G обозначают пустую клетку, остальные - препятствие.

Простой файл со сценариями выглядит следующим образом:

version 1<br>
0 brc202d.map 3 3 0 1 2 1 2<br>
0 brc202d.map 3 3 1 0 1 2 2<br>

3-я переменная: высота карты, 4-ая переменная - ширина карты, 5-ая и 6-ая переменные - стартовые x и y, 7-ая и 8-ая переменные - финишные x и y, 9-ая переменная - оптимальная длина пути.
