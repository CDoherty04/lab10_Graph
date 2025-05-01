g++ -std=c++11 MainTest.cpp -o myProgram
for i in $(seq 0 9); do ./myProgram Inputs/input_vertex_$i.txt Inputs/input_edge_$i.txt > Results/result_$i.txt; done
for i in $(seq 0 9); do python3 GradingScript.py Results/result_$i.txt Outputs/output_$i.txt; done