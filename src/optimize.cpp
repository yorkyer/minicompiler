#include "utils.h"
#include "mips_impl.h"
#include "optimize_impl.h"


static ofstream output;
static int index;
static vector<pair<string, vector<int>>> compute_graph;
static vector<pair<string, int>> io_node, write_node;

static map<string, int> var2node;
static map<pair<string, vector<int>>, int> op2node;
static int call_node;

static vector<string> node2var;
static bool isnum(char c);


void optimize()
{
	output.open("optimize.txt");

	get_bb();

	output.close();
}


static bool isnum(char c)
{
	return isdigit(c) || c == '-';
}


static vector<string> parse(string line)
{
	vector<string> tokens;
	stringstream str(line);
	string word;
	while (str >> word) {
		tokens.push_back(word);
	}
	return tokens;
}


static void get_bb()
{
	ifstream input("code.txt");
	string line;
	vector<vector<string>> block;
	while (getline(input, line)) {
		if (line.empty())
			continue;

		vector<string> tokens = parse(line);

		if (tokens[0] == "FUNCTION" || tokens[0] == "IF" || tokens[0] == "GOTO" ||
				tokens[0] == "M[" || tokens[0] == "RET" || tokens[0].back() == ':') {
			if (!block.empty()) {
				if (tokens[0] == "M[")
					block.push_back(tokens);

#ifdef DEBUG
				cout << "==============new block====================\n"; 
				for (auto tokens : block) {
					for (auto token : tokens)
						cout << token << " ";
					cout << "\n";
				}
				cout << "\n\n\n";
#endif

				bb2graph(block);
				block.clear();				
			}

			if (tokens[0] == "IF") {
				string v1, v2;
				if (!isnum(tokens[1][0])) 
					v1 = compute(var2node[tokens[1]]);
				else
					v1 = tokens[1];
				if (!isnum(tokens[3][0]))
					v2 = compute(var2node[tokens[3]]);
				else
					v2 = tokens[3];
				string op = tokens[2];
				if (isnum(v1[0]) && isnum(v2[0])) {
					bool istrue;
					if (op == ">") istrue = stoi(v1) > stoi(v2);
					else if (op == ">=") istrue = stoi(v1) >= stoi(v2);
					else if (op == "<") istrue = stoi(v1) < stoi(v2);
					else if (op == ">=") istrue = stoi(v1) >= stoi(v2);
					else if (op == "==") istrue = stoi(v1) == stoi(v2);
					else istrue = stoi(v1) != stoi(v2);

					if (istrue) output << tokens[4] << " " << tokens[5] << "\n";
				}
        else if (isnum(v1[0]))
					output << tokens[0] << " " << v2 << " " << op << " " << v1 << " "
							   << tokens[4] << " " << tokens[5] << "\n";
				else
					output << tokens[0] << " " << v1 << " " << op << " " << v2 << " "
							   << tokens[4] << " " << tokens[5] << "\n";
			}
			else if (tokens[0] != "M[") {
				for (auto token : tokens)
					output << token << " ";
				output << "\n";
			}
		} 
		else {
			block.push_back(tokens);
		}
	}
	input.close();
}



static int leaf_node(string var)
{
	if (var2node.count(var) == 0 && var != "v0") {
		vector<int> vec;
		compute_graph.push_back(make_pair(var, vec));
		var2node[var] = compute_graph.size() - 1;
	}
	else if (var == "v0") {
		return call_node;
	}
	return var2node[var];
}


static int intern_node(string op, string var1, string var2)
{
	int node1 = leaf_node(var1);
	int node2 = leaf_node(var2);
	vector<int> vec = { node1, node2 };
	if (op2node.count(make_pair(op, vec)) == 0) {
		compute_graph.push_back(make_pair(op, vec));
		op2node[make_pair(op, vec)] = compute_graph.size() - 1;
	}
	return op2node[make_pair(op, vec)];
}


static void bb2graph(vector<vector<string>>& block)
{
	index = 0;
	compute_graph.clear();
	io_node.clear();
	write_node.clear();

	var2node.clear();
	op2node.clear();

	for (auto line : block) {
		if (line.size() == 0)
			continue;
		if (line[0] == "CALL") {
			vector<int> args = { -1 };
			for (int i = 2; i < line.size(); i++) {
				int node = leaf_node(line[i]);
				args.push_back(node);
			}
			compute_graph.push_back(make_pair(line[1], args));
			io_node.push_back(make_pair("call", compute_graph.size() - 1));
			call_node = compute_graph.size() - 1;
		}
		else if (line.size() == 3) {
			if (line[0] == "PRINT") {
				int node = leaf_node(line[2]);
				if (line[1] == "INT")
					io_node.push_back(make_pair("oi", node));
				else if (line[1] == "CHR")
					io_node.push_back(make_pair("oc", node));
				else
					io_node.push_back(make_pair("os", node));
			}
			else if (line[0] == "READ") {
				vector<int> vec;
				compute_graph.push_back(make_pair("", vec));
				var2node[line[2]] = leaf_node(line[2]);
				if (line[1] == "INT") 
					io_node.push_back(make_pair("ii", compute_graph.size() - 1));
				else
					io_node.push_back(make_pair("ic", compute_graph.size() - 1));
			}
			else {
				var2node[line[0]] = leaf_node(line[2]);
				if (line[0][0] == 's' || line[0][0] == 'g' || line[0][0] == 'a' || line[0][0] == 'v') {
					write_node.push_back(make_pair(line[0], var2node[line[0]]));
				}
			}
		}
		else if (line[0][0] == 'M') {
			int node1 = leaf_node(line[1]);
			int node2 = leaf_node(line[4]);
			write_node.push_back(make_pair("M"+to_string(node1), node2));
		}
		else if (line[2][0] == 'M') {
			int node = leaf_node(line[3]);
			vector<int> vec = { node };
			if (op2node.count(make_pair("", vec)) == 0) {
				compute_graph.push_back(make_pair("", vec));
				op2node[make_pair("", vec)] = compute_graph.size() - 1;
			}
			var2node[line[0]] = op2node[make_pair("", vec)];
			if (line[0][0] == 's' || line[0][0] == 'g' || line[0][0] == 'a' || line[0][0] == 'v') {
				write_node.push_back(make_pair(line[0], var2node[line[0]]));
			}
		}
		else {
			var2node[line[0]] = intern_node(line[3], line[2], line[4]);
			if (line[0][0] == 's' || line[0][0] == 'g' || line[0][0] == 'a' || line[0][0] == 'v') {
				write_node.push_back(make_pair(line[0], var2node[line[0]]));
			}
		}
	}

	graph2bb();
}


static string new_var()
{
	return "t" + to_string(index++);
}


static string compute(int node)
{
	if (compute_graph[node].second.empty()) { // ("", []), (10, []), (s0, []), (f, []) -> (f, [-1])
		if (compute_graph[node].first.empty()) {
			string var = new_var();
			node2var[node] = var;
			return var;
		}
		else {
			node2var[node] = compute_graph[node].first;
			return compute_graph[node].first;
		}
	}

	if (!node2var[node].empty())
		return node2var[node];

	vector<string> vars;
	for (auto n : compute_graph[node].second) {
		if (n == -1) 
			continue;
		vars.push_back(compute(n));
	}

	string var = new_var();
	if (compute_graph[node].first == "")
		output << var << " = " << "M[ " << vars[0] << " ]\n";
	else if (compute_graph[node].first == "+" || compute_graph[node].first == "-" ||
					 compute_graph[node].first == "*" || compute_graph[node].first == "/") {
		if (isnum(vars[0][0]) && isnum(vars[1][0])) {
			string right;
			if (compute_graph[node].first == "+")
				right = to_string(stoi(vars[0]) + stoi(vars[1]));
			else if (compute_graph[node].first == "-")
				right = to_string(stoi(vars[0]) - stoi(vars[1]));
			else if (compute_graph[node].first == "*")
				right = to_string(stoi(vars[0]) * stoi(vars[1]));
			else if (compute_graph[node].first == "/")
				right = to_string(stoi(vars[0]) / stoi(vars[1]));
			output << var << " = " << right << "\n";
		}
		else if (isnum(vars[0][0])) {
			output << var << " = " << vars[1] << " " << compute_graph[node].first
											<< " " << vars[0] << "\n";	
		}
		else {
			output << var << " = " << vars[0] << " " << compute_graph[node].first
									    << " " << vars[1] << "\n";
		}
	}
	node2var[node] = var;
	return var;
}


static void graph2bb()
{
#ifdef DEBUG
	cout << "\ncompute_graph: \n";
	for (auto node : compute_graph) {
		cout << node.first << ": ";
		for (auto child : node.second) {
			cout << child << " ";
		}
		cout << "\n";
	}

	cout << "\nio_node: \n";
	for (auto node : io_node) {
		cout << node.first << ": " << node.second << "\n";
	}

	cout << "\nwrite_node: \n";
	for (auto node : write_node) {
		cout << node.first << ": " << node.second << "\n";
	}

	cout << "\nvar2node: \n";
	for (auto node : var2node) {
		cout << node.first << ": " << node.second << "\n";
	}	

	cout << "\nop2node: \n";
	for (auto node : op2node) {
		cout << node.first.first << " ";
		for (auto child : node.first.second) {
			cout << child << " ";
		}
		cout << ": " << node.second << "\n";
	}	
#endif

	node2var.clear();
	node2var.resize(compute_graph.size());

	for (auto node : io_node) {
		string var = compute(node.second);
		if (node.first == "oi") {
			output << "PRINT INT " << var << "\n";
		}
		else if (node.first == "oc") {
			output << "PRINT CHR " << var << "\n";
		}
		else if (node.first == "os") {
			output << "PRINT STR " << var << "\n";
		}
		else if (node.first == "ii") {
			output << "READ INT " << var << "\n";
		}
		else if (node.first == "ic") {
			output << "PRINT CHR " << var << "\n";
		}
		else {
			output << "CALL " << compute_graph[node.second].first << " ";
			for (auto arg : compute_graph[node.second].second) {
				if (arg == -1) 
					continue;
				output << node2var[arg] << " ";
			}
			output << "\n";
			output << var << " = " << "v0\n";
		}
	}

	for (auto node : write_node) {
		string var = compute(node.second);
		if (node.first[0] != 'M')
			output << node.first << " = " << var << "\n";
		else {
			string address = compute(stoi(node.first.substr(1)));
			output << "M[ " << address << " ] = " << var << "\n";
		}
	}

	output << "\n";

#ifdef DEBUG
	cout << "\nnode2var: \n";
	for (int i = 0; i < node2var.size(); i++) {
		cout << i << ": " << node2var[i] << "\n";
	}
#endif
}


// int main(int argc, char const *argv[])
// {
// 	output.open("optimize.txt");

// 	get_bb();

// 	output.close();

// 	return 0;
// }







