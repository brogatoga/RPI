@echo off
g++ *.cpp -o graph.exe

graph.exe inputs/graph_test.txt > results/graph.txt
diff results/graph.txt outputs/out_graph_test.txt
graph.exe inputs/message_test.txt > results/message.txt
graph.exe inputs/remove_test.txt  > results/remove.txt
diff results/remove.txt outputs/out_remove_test.txt
graph.exe inputs/custom_test.txt  > results/custom.txt

rm graph.exe