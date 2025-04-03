# Graph Parameter Analyzer

This program processes multiple graphs and determines key structural properties, including:
 - Degree sequence
 - Number of connected components
 - Bipartiteness check
 - TBD


Input Format

The first line contains an integer k, representing the number of graphs. Each of the next k data sets describes a graph:
 - The first value is n, the number of vertices.
 - The next n lines describe the adjacency list of each vertex:
    - Each line starts with an integer s, the number of neighbors.
    - Followed by s space-separated integers representing neighbor IDs (ranging from 1 to n).


Output Format

For each of the k graphs, the program prints the required parameters in a structured format:
 - Degree sequence – list of n integers.
 - Number of components – single integer.
 - Bipartiteness check – "T" (True) or "F" (False).