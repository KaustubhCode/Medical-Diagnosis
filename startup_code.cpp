#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <time.h> 

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

//Helper functions

//Returns random number between a and b where a and b are in (0,1)
float randRange(float a, float b){
	float num = ((float)rand())/RAND_MAX;
	return (a + num*(b-a));
}

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

private:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning

public:
	// Constructor- a node is initialised with its name and its categories
	Graph_Node(string name,int n,vector<string> vals){
		Node_Name=name;
		nvalues=n;
		values=vals;
	}
	string get_name(){
		return Node_Name;
	}
	vector<int> get_children(){
		return Children;
	}
	vector<string> get_Parents(){
		return Parents;
	}
	vector<float> get_CPT(){
		return CPT;
	}
	int get_nvalues(){
		return nvalues;
	}
	vector<string> get_values(){
		return values;
	}
	void set_CPT(vector<float> new_CPT){
		CPT.clear();
		CPT=new_CPT;
	}
	void set_Parents(vector<string> Parent_Nodes){
		Parents.clear();
		Parents=Parent_Nodes;
	}
	// add another node in a graph as a child of this node
	int add_child(int new_child_index ){
		for(int i=0;i<Children.size();i++)
		{
			if(Children[i]==new_child_index){
				return 0;
			}
		}
		Children.push_back(new_child_index);
		return 1;
	}

	void printNode(){
		cout << "Variable Name: "<< Node_Name << endl;
		cout << "Children:" << endl;
		for (int i = 0; i < Children.size(); i++){
			cout << Children[i] << " ";
		}
		cout << endl;
		cout << "Parents:" << endl;
		for (int i = 0; i < Parents.size(); i++){
			cout << Parents[i] << " ";
		}
		cout << endl;
		cout << "Categories:" << endl;
		for (int i = 0; i < values.size(); i++){
			cout << values[i] << " ";
		}
		cout << endl;
		cout << "CPT:" << endl;
		for (int i = 0; i < CPT.size(); i++){
			cout << CPT[i] << " ";
		}
		cout << endl;
		cout << "-------------------------------------" << endl;
	}

	void random(){
		//Assign random CPT's
		int n = Parents.size();
		int m = nvalues;
		int cpt_size = pow(2,n)*m;
		vector<float> new_cpt(cpt_size, -1);
 		
 		for (int i = 0; i < pow(2,n); i++){
 			vector<float> temp;
 			float sum = 1;
	 		for (int j = 0; j < m; j++){
	 			if (j == m-1){
	 				new_cpt[j*pow(2,n) + i] = sum;	
	 				break;
	 			}
	 			float k = randRange(0,sum);
	 			// cout << k << " ";
	 			new_cpt[j*pow(2,n) + i] = k;
	 			sum = sum - k;
	 		}
 		}

 		CPT = new_cpt;
	}
};


 // The whole network represted as a list of nodes
class network{

	list <Graph_Node> Pres_Graph;

public:
	int addNode(Graph_Node node){
		Pres_Graph.push_back(node);
		return 0;
	}
	
	
	int netSize(){
		return Pres_Graph.size();
	}

	// get the index of node with a given name
	int get_index(string val_name){
		list<Graph_Node>::iterator listIt;
		int count=0;
		for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
		{
			if(listIt->get_name().compare(val_name)==0){
				return count;
			}
			count++;
		}
		return -1;
	}

	// get the node at nth index
	list<Graph_Node>::iterator get_nth_node(int n){
		list<Graph_Node>::iterator listIt;
		int count=0;
		for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
		{
			if(count==n){
				return listIt;
			}
			count++;
		}
		return listIt; 
	}
	//get the iterator of a node with a given name
	list<Graph_Node>::iterator search_node(string val_name){
		list<Graph_Node>::iterator listIt;
		for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
		{
			if(listIt->get_name().compare(val_name)==0){
				return listIt;
			}
		}
		cout<<"node not found\n";
		return listIt;
	}

	void printNetwork(){
		list<Graph_Node>::iterator listIt;
		for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
		{
			listIt->printNode();
		}
	}

	//Creates CPT randomly
	void initialise(){
		list<Graph_Node>::iterator listIt;
		for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
		{
			listIt->random();
		}
	}

	//Return Probability value P(X_i=val|Parents(X)) (Assuming, parent values are given in order)
	float getProb(int i, string val, vector<string> parent_vals){
		list<Graph_Node>::iterator it = get_nth_node(i);
		vector<float> cpt = it->get_CPT();
		vector<string> parents = it->get_Parents();
		int idx = 0;
		int n = parent_vals.size();
		for (int j = n-1; j >= 0; j++){
			list<Graph_Node>::iterator temp = search_node(parents[i]);
			vector<string> temp_val = temp->get_values();
			for (int k = 0; k < temp_val.size(); k++){
				if (temp_val[k] == parent_vals[j]){
					idx = idx + k*pow(temp_val.size(),n-j-1);
				}
			}
		}

		return cpt[idx];
	}


	//Fill record with appropriate value
	vector<string> fill(int i, vector<string> record){
		// Get categories to be filled
		// float prob = findprob(i,&record);
		return record;
	}
};

network read_network()
{
	network Alarm_new;
	string line;
	int find=0;
	ifstream myfile("alarm.bif"); 
	string temp;
	string name;
	vector<string> values;
	
	if (myfile.is_open())
	{
		while (! myfile.eof() )
		{
			stringstream ss;
			getline (myfile,line);
			
			ss.str(line);
			ss>>temp;
			
			if(temp.compare("variable")==0)
			{
				ss>>name;
				getline (myfile,line);
				stringstream ss2;
				ss2.str(line);
				for(int i=0;i<4;i++)
				{
					ss2>>temp;
				}
				values.clear();
				while(temp.compare("};")!=0)
				{
					values.push_back(temp);
					ss2>>temp;
				}
				Graph_Node new_node(name,values.size(),values);
				int pos=Alarm_new.addNode(new_node);
			}
			else if(temp.compare("probability")==0)
			{
				ss>>temp;
				ss>>temp;
				
				list<Graph_Node>::iterator listIt;
				list<Graph_Node>::iterator listIt1;
				listIt=Alarm_new.search_node(temp);
				int index=Alarm_new.get_index(temp);
				ss>>temp;
				values.clear();
				while(temp.compare(")")!=0)
				{
					listIt1=Alarm_new.search_node(temp);
					listIt1->add_child(index);
					values.push_back(temp);
					ss>>temp;
				}
				listIt->set_Parents(values);

				getline (myfile,line);
				stringstream ss2;
				
				ss2.str(line);
				ss2>> temp;
				ss2>> temp;
				
				vector<float> curr_CPT;
				string::size_type sz;
				while(temp.compare(";")!=0)
				{
					curr_CPT.push_back(atof(temp.c_str()));
					ss2>>temp;
				}
				listIt->set_CPT(curr_CPT);
			}
			else{}
		}
		
		if(find==1)
		myfile.close();
	}
	
	return Alarm_new;
}

//Global variables
int values = 37; //Fixed or flexible?
vector<vector<string> > v;
vector<vector<vector<string> > > records(values+1, v);
// vector<vector<string> > records;
network Alarm;

int main()
{
	srand(time(0));
	Alarm=read_network();
	values = Alarm.netSize();

	int count;
	string temp;
	//Reading records.dat
	ifstream myfile("records.dat");

	if (myfile.is_open()){
		while( !myfile.eof()){
			stringstream ss;
			string line;
			vector<string> rec;
			getline (myfile,line);
			ss.str(line);

			count = -1;
			for (int i = 0; i < values;i++){
				ss >> temp;
				temp = temp.substr(1,temp.length()-2);
				// cout << temp << " ";
				if (temp == "?"){
					count = i;
				}
				rec.push_back(temp);
			}
			count = (count == -1) ? 38 : count;
			records[count].push_back(rec);
		}
	}

	// int sum = 0;
	// for (int i = 0; i < 38; i++){
	// 	cout << records[i].size() << endl;
	// 	sum = sum + records[i].size();
	// }	
	// cout << sum << endl;

	// // Printing all records
	// for (int k = 0; k < 38; k++){
	// 	for (int i = 0; i < records[k].size(); i++){
	// 		for (int j = 0; j < records[k][i].size(); j++){
	// 			cout << records[k][i][j] << " ";
	// 		}
	// 		cout << endl;
	// 		cout << endl;
	// 	}
	// }

	//Initialisation of network
	Alarm.initialise();
	Alarm.printNetwork();

	int max_iter = 1;
	int iter = 0;
	// Run till some condition is satisfied
	while (iter != max_iter){
		//E-step
		//Inference of each variable
		vector<float> probs(values,0);
		for (int i = 0; i < values; i++){
			for (int j = 0; j < records[i].size(); j++){
				records[i][j] = Alarm.fill(i,records[i][j]);
			}
		}


		//M-step
		//Use counting to get actual prob values

		iter++;
	}
	
}





