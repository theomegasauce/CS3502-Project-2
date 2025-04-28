#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <vector>
#include <climits>

struct Process {
    int id, arrival, burst, priority, remaining;
    int start_time = -1, completion_time = 0;
};

struct Metrics {
    double avg_waiting_time, avg_turnaround_time, cpu_utilization, throughput;
};

void reset(std::vector<Process>& procs) {
    for (auto &p : procs) {
        p.remaining = p.burst;
        p.start_time = -1;
        p.completion_time = 0;
    }
}

Metrics compute_metrics(const std::vector<Process>& procs, int total_time) {
    int n = procs.size();
    double total_wait = 0, total_turn = 0;
    int busy_time = 0;
    
    for (const auto &p : procs) {
        int turnaround = p.completion_time - p.arrival;
        total_turn += turnaround;
        total_wait += turnaround - p.burst;
        busy_time += p.burst;
    }
    
    return {
        total_wait / n,
        total_turn / n,
        100.0 * busy_time / total_time,
        double(n) / total_time
    };
}

Metrics fcfs(std::vector<Process> procs, int cs_time = 0) {
    reset(procs);
    std::sort(procs.begin(), procs.end(), [](auto &a, auto &b){ return a.arrival < b.arrival; });
    
    int time = 0;
    int prev_id = -1; 
    
    for (auto &p : procs) {
        time = std::max(time, p.arrival);
        
        if (prev_id != -1) {
            time += cs_time;
        }
        
        p.start_time = time;
        time += p.burst;
        p.completion_time = time;
        prev_id = p.id;
    }
    return compute_metrics(procs, time);
}

Metrics sjf(std::vector<Process> procs, int cs_time = 0) {
    reset(procs);
    int time = 0, completed = 0, n = procs.size();
    std::vector<bool> done(n, false);
    int prev_id = -1;
    
    while (completed < n) {
        int idx = -1, min_burst = INT_MAX;
        for (int i = 0; i < n; ++i) {
            if (!done[i] && procs[i].arrival <= time && procs[i].burst < min_burst) {
                min_burst = procs[i].burst;
                idx = i;
            }
        }
        
        if (idx < 0) { time++; continue; }
        
        auto &p = procs[idx];
        
        if (prev_id != -1) {
            time += cs_time;
        }
        
        p.start_time = time;
        time += p.burst;
        p.completion_time = time;
        done[idx] = true;
        completed++;
        prev_id = p.id;
    }
    return compute_metrics(procs, time);
}

Metrics srtf(std::vector<Process> procs, int cs_time = 0) {
    reset(procs);
    int time = 0, completed = 0, n = procs.size();
    int current_process = -1; 
    
    while (completed < n) {
        int idx = -1, min_rem = INT_MAX;
        for (int i = 0; i < n; ++i) {
            if (procs[i].arrival <= time && procs[i].remaining > 0 && procs[i].remaining < min_rem) {
                min_rem = procs[i].remaining;
                idx = i;
            }
        }
        
        if (idx < 0) { time++; continue; }
        
        if (current_process != idx && current_process != -1) {
            time += cs_time;
        }
        
        auto &p = procs[idx];
        current_process = idx;
        
        if (p.start_time == -1) p.start_time = time;
        p.remaining--;
        time++;
        
        if (p.remaining == 0) {
            p.completion_time = time;
            completed++;
            current_process = -1; 
        }
    }
    return compute_metrics(procs, time);
}

Metrics round_robin(std::vector<Process> procs, int quantum, int cs_time = 0) {
    reset(procs);
    int n = procs.size();
    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int a, int b) { return procs[a].arrival < procs[b].arrival; });
    
    std::queue<int> ready;
    int time = 0, completed = 0, idx = 0;
    int current_process = -1; 
    
    while (completed < n) {
        while (idx < n && procs[order[idx]].arrival <= time) {
            ready.push(order[idx++]);
        }
        
        if (ready.empty()) {
            time = procs[order[idx]].arrival;
            continue;
        }
        
        int cur = ready.front();
        ready.pop();
        
        if (current_process != cur && current_process != -1) {
            time += cs_time;
        }
        
        current_process = cur;
        
        if (procs[cur].start_time == -1) procs[cur].start_time = time;
        
        int run = std::min(quantum, procs[cur].remaining);
        procs[cur].remaining -= run;
        time += run;
        
        while (idx < n && procs[order[idx]].arrival <= time) {
            ready.push(order[idx++]);
        }
        
        if (procs[cur].remaining == 0) {
            procs[cur].completion_time = time;
            completed++;
            current_process = -1; 
        } else {
            if (!ready.empty()) {
                time += cs_time;
            }
            ready.push(cur);
        }
    }
    return compute_metrics(procs, time);
}

Metrics priority_np(std::vector<Process> procs, int cs_time = 0) {
    reset(procs);
    int time = 0, completed = 0, n = procs.size();
    std::vector<bool> done(n, false);
    int prev_id = -1;
    
    while (completed < n) {
        int idx = -1, best_priority = INT_MAX;
        for (int i = 0; i < n; ++i) {
            if (!done[i] && procs[i].arrival <= time && procs[i].priority < best_priority) {
                best_priority = procs[i].priority;
                idx = i;
            }
        }
        
        if (idx < 0) { time++; continue; }
        
        auto &p = procs[idx];
        
        if (prev_id != -1) {
            time += cs_time;
        }
        
        p.start_time = time;
        time += p.burst;
        p.completion_time = time;
        done[idx] = true;
        completed++;
        prev_id = p.id;
    }
    return compute_metrics(procs, time);
}

Metrics hrrn(std::vector<Process> procs, int cs_time = 0) {
    reset(procs);
    int time = 0, completed = 0, n = procs.size();
    std::vector<bool> done(n, false);
    int prev_id = -1;
    
    while (completed < n) {
        int idx = -1;
        double best_rr = -1.0;
        
        for (int i = 0; i < n; ++i) {
            if (!done[i] && procs[i].arrival <= time) {
                double rr = (time - procs[i].arrival + procs[i].burst) / static_cast<double>(procs[i].burst);
                if (rr > best_rr) {
                    best_rr = rr;
                    idx = i;
                }
            }
        }
        
        if (idx < 0) { time++; continue; }
        
        auto &p = procs[idx];
        
        if (prev_id != -1) {
            time += cs_time;
        }
        
        p.start_time = time;
        time += p.burst;
        p.completion_time = time;
        done[idx] = true;
        completed++;
        prev_id = p.id;
    }
    return compute_metrics(procs, time);
}

Metrics mlfq(std::vector<Process> procs, const std::vector<int>& quantums, int cs_time = 0) {
    reset(procs);
    int n = procs.size(), levels = quantums.size();
    std::vector<std::queue<int>> queues(levels);
    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int a, int b) { return procs[a].arrival < procs[b].arrival; });
    
    int next_arrival = 0, time = 0, completed = 0;
    int current_process = -1;
    
    while (completed < n) {
        while (next_arrival < n && procs[order[next_arrival]].arrival <= time) {
            queues[0].push(order[next_arrival++]);
        }
        
        int lvl = -1;
        for (int i = 0; i < levels && lvl < 0; ++i) {
            if (!queues[i].empty()) lvl = i;
        }
        
        if (lvl < 0) {
            time = next_arrival < n ? procs[order[next_arrival]].arrival : time + 1;
            continue;
        }
        
        int idx = queues[lvl].front();
        queues[lvl].pop();
        
        if (current_process != idx && current_process != -1) {
            time += cs_time;
        }
        
        current_process = idx;
        
        auto &p = procs[idx];
        if (p.start_time == -1) p.start_time = time;
        
        int run = std::min(quantums[lvl], p.remaining);
        p.remaining -= run;
        time += run;
        
        while (next_arrival < n && procs[order[next_arrival]].arrival <= time) {
            queues[0].push(order[next_arrival++]);
        }
        
        if (p.remaining == 0) {
            p.completion_time = time;
            completed++;
            current_process = -1;
        } else {
            time += cs_time;
            queues[std::min(lvl + 1, levels - 1)].push(idx);
        }
    }
    return compute_metrics(procs, time);
}

int main() {
    std::vector<Process> processes_identical_burst = {
        { 101, 0, 10, 3, 10 }, { 102, 0, 10, 1, 10 }, { 103, 0, 10, 5, 10 }, 
        { 104, 0, 10, 2, 10 }, { 105, 0, 10, 4, 10 }, { 106, 0, 10, 2, 10 }, 
        { 107, 0, 10, 1, 10 }, { 108, 0, 10, 3, 10 }, { 109, 0, 10, 4, 10 }, 
        { 110, 0, 10, 3, 10 }
    };

    std::vector<Process> processes_varied_burst = {
        { 201, 0, 100, 3, 100 }, { 202, 0, 2, 1, 2 }, { 203, 0, 150, 5, 150 }, 
        { 204, 0, 1, 2, 1 }, { 205, 0, 120, 4, 120 }, { 206, 0, 3, 2, 3 }, 
        { 207, 0, 80, 1, 80 }, { 208, 0, 2, 3, 2 }, { 209, 0, 90, 4, 90 }, 
        { 210, 0, 1, 3, 1 }
    };

    std::vector<Process> processes_skewed_priorities = {
        { 301, 0, 15, 1, 15 }, { 302, 0, 12, 9, 12 }, { 303, 0, 18, 2, 18 }, 
        { 304, 0, 10, 10, 10 }, { 305, 0, 20, 1, 20 }, { 306, 0, 8, 8, 8 }, 
        { 307, 0, 25, 3, 25 }, { 308, 0, 5, 7, 5 }, { 309, 0, 14, 1, 14 }, 
        { 310, 0, 22, 6, 22 }
    };

    auto test_process_set = [](const std::string& set_name, const std::vector<Process>& processes, int context_switch_time = 1) {
        std::cout << "\n====== Testing " << set_name << " ======\n";
        
        const int rr_quantum = 4;
        const std::vector<int> mlfq_quantums = {4, 8, 16};

        auto m_fcfs = fcfs(processes, context_switch_time);
        auto m_sjf  = sjf(processes, context_switch_time);
        auto m_srtf = srtf(processes, context_switch_time);
        auto m_rr   = round_robin(processes, rr_quantum, context_switch_time);
        auto m_prio = priority_np(processes, context_switch_time);
        auto m_hrrn = hrrn(processes, context_switch_time);
        auto m_mlfq = mlfq(processes, mlfq_quantums, context_switch_time);

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Algorithm      AWT    ATT    CPU%   Throughput  (CS Time: " << context_switch_time << ")\n";
        std::cout << "--------------------------------------------------------------------\n";
        
        auto print = [](const std::string &name, const Metrics &m) {
            std::cout << std::setw(12) << name
                      << std::setw(8)  << m.avg_waiting_time
                      << std::setw(7)  << m.avg_turnaround_time
                      << std::setw(7)  << m.cpu_utilization
                      << std::setw(10) << m.throughput
                      << "\n";
        };
        
        print("FCFS",     m_fcfs);
        print("SJF",      m_sjf);
        print("SRTF",     m_srtf);
        print("RR",       m_rr);
        print("Priority", m_prio);
        print("HRRN",     m_hrrn);
        print("MLFQ",     m_mlfq);
    };

    std::cout << "\n\n===============================================================";
    std::cout << "\n                TESTS WITH CONTEXT SWITCH TIME = 1";
    std::cout << "\n===============================================================\n";
    
    test_process_set("Identical Burst Times", processes_identical_burst);
    test_process_set("Varied Burst Times", processes_varied_burst);
    test_process_set("Skewed Priorities", processes_skewed_priorities);

    std::cout << "\n\n===============================================================";
    std::cout << "\n                TESTS WITH CONTEXT SWITCH TIME = 0";
    std::cout << "\n===============================================================\n";
    
    test_process_set("Identical Burst Times", processes_identical_burst, 0);
    test_process_set("Varied Burst Times", processes_varied_burst, 0);
    test_process_set("Skewed Priorities", processes_skewed_priorities, 0);

    return 0;
}