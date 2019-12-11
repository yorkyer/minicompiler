#ifndef OPTIMIZE_IMPL_H
#define OPTIMIZE_IMPL_H


void optimize();
static vector<string> parse(string line);
static void get_bb();
static int leaf_node(string var);
static int intern_node(string op, string var1, string var2);
static void bb2graph(vector<vector<string>>& block);
static string new_var();
static string compute(int node);
static void graph2bb();


#endif