# CS3502-Project-2
# CPU Scheduling Simulator

This project simulates various **CPU scheduling algorithms** and computes key performance metrics such as:

- Average Waiting Time (AWT)
- Average Turnaround Time (ATT)
- CPU Utilization (%)
- Throughput (Processes per second)

The simulation includes the following algorithms:
- **FCFS** (First Come First Serve)
- **SJF** (Shortest Job First)
- **SRTF** (Shortest Remaining Time First)
- **Round Robin (RR)**
- **Priority Scheduling**
- **HRRN** (Highest Response Ratio Next)
- **MLFQ** (Multilevel Feedback Queue)

---

## Features
- Simulates **process execution** based on arrival time, burst time, and priority.
- Tracks **context switching time** (optional parameter).
- Outputs performance metrics after simulation.
- Modular design for easy extension with new scheduling algorithms.

## File Overview
- `test3.c++`: Core implementation file containing:
  - `Process` struct to represent process attributes.
  - `Metrics` struct for performance analysis.
  - Functions for each CPU scheduling strategy (FCFS, SJF, SRTF).
  - Utility functions to reset processes and calculate final statistics.

## Usage

1. **Compile** the program:
    ```bash
    g++ -o scheduler test3.c++
    ```

2. **Run** the executable:
    ```bash
    ./scheduler
    ```

3. **Example** `main()` function:

    ```cpp
    #include <iostream>
    #include <vector>
    // Include "test3.c++" or split into headers appropriately.

    int main() {
        std::vector<Process> processes = {
            {1, 0, 5, 2},
            {2, 1, 3, 1},
            {3, 2, 8, 3}
        };
        
        auto metrics = fcfs(processes, 1); // context switch time = 1
        std::cout << "Average Waiting Time: " << metrics.avg_waiting_time << std::endl;
        std::cout << "Average Turnaround Time: " << metrics.avg_turnaround_time << std::endl;
        std::cout << "CPU Utilization: " << metrics.cpu_utilization << "%" << std::endl;
        std::cout << "Throughput: " << metrics.throughput << " processes/sec" << std::endl;
        
        return 0;
    }
    ```


