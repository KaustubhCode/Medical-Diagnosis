#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <time.h> 
#include <iomanip>
#include <unordered_map>

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

//Global Variables
unordered_map<string, int> string_to_idx;
unordered_map<int, string> idx_to_string;

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
		int cpt_size = CPT.size();
		int parent_combinations = CPT.size()/nvalues;
		vector<float> new_cpt(cpt_size, -1);
 		
 		for (int i = 0; i < parent_combinations; i++){
			float sum = 1;
			for (int j = 0; j < nvalues; j++){
				if (j==nvalues-1){
					new_cpt[j*parent_combinations+i] = sum;
					break;
				}
				float k = randRange(0,sum);
				new_cpt[j*parent_combinations+i] = k;
				sum = sum-k;
			} 			
 		}

 		CPT = new_cpt;
	}
};


 // The whole network represented as a vector of nodes
class network{

	vector<Graph_Node> Pres_Graph;

public:
	int addNode(Graph_Node node){
		Pres_Graph.push_back(node);
		return 0;
	}
	
	
	int netSize(){
		return Pres_Graph.size();
	}

	// get the node at nth index
	Graph_Node get_nth_node(int n){
		return Pres_Graph[n]; 
	}

	//get the iterator of a node with a given name
	vector<Graph_Node>::iterator search_node(string val_name){
		vector<Graph_Node>::iterator listIt;
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
		vector<Graph_Node>::iterator listIt;
		for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
		{
			listIt->printNode();
		}
	}

	//Creates CPT randomly
	void initialise(){
		vector<Graph_Node>::iterator listIt;
		for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
		{
			listIt->random();
		}
	}

	void printGraph(){
		int n = Pres_Graph.size();
		for (int i = 0; i < n; i++){
			cout << i << ":" << Pres_Graph[i].get_name() << " ";
		}
	}

	//Return Probability value P(X_i=val|Parents(X)) (Assuming parent_vals is in order)
	float getProb(int i, string val, vector<string> parent_vals){
		Graph_Node it = get_nth_node(i);
		vector<float> cpt = it.get_CPT();

		if (parent_vals.size() == 0){
			vector<string> temp_val = it.get_values();
			for (int k = 0; k < temp_val.size(); k++){
				if (temp_val[k] == val){
					return cpt[k];
				}
			}
			cout << "Error. No matching value found." << endl;
		}


		vector<string> parents = it.get_Parents();
		int n = parent_vals.size();
		int idx = 0;
		int base = 1;
		for (int j = n-1; j >= 0; j--){
			Graph_Node temp = get_nth_node(string_to_idx[parents[j]]);
			vector<string> temp_val = temp.get_values();
			for (int k = 0; k < temp_val.size(); k++){
				if (temp_val[k] == parent_vals[j]){
					idx = idx + k*base;
				}
			}
			base = base * temp_val.size();
		}

		vector<string> temp_val = it.get_values();
		for (int k = 0; k < temp_val.size(); k++){
			if (temp_val[k] == val){
				idx = idx + k*base;
			}
		}

		return cpt[idx];
	}

	float markovBlanket(int i, string val, vector<string> record){
		Graph_Node it = get_nth_node(i);
		string name = it.get_name();
		vector<string> parents = it.get_Parents();
		vector<int> children = it.get_children();

		float ans = 1;
		vector<string> temp_values(parents.size(),"");

		for (int j = 0; j < parents.size(); j++){
			temp_values[j] = record[string_to_idx[parents[j]]];
		}

		// cout << i << " " << val << " " << parents.size() << " " << endl;
		// for (int j = 0; j < parents.size(); j++){
		// 	cout << parents[j] << ":" << temp_values[j] << endl;
		// }

		ans = ans * getProb(i, val, temp_values); 



		for (int j = 0; j < children.size(); j++){
			Graph_Node child_node = get_nth_node(children[j]);
			vector<string> parents_of_child = child_node.get_Parents();
			vector<string> temp_values(parents_of_child.size(),"");
			for (int k = 0; k < parents_of_child.size(); k++){
				if (parents_of_child[k] == val){
					temp_values[k] = val;
					continue;
				}
				temp_values[k] = record[string_to_idx[parents_of_child[k]]];
			}
			ans = ans * getProb(children[j], record[children[j]], temp_values);
		}

		// cout << "Markov ans: " << ans << endl; 
		return ans;
	}


	//Fill record with appropriate value
	vector<string> fill(int i, vector<string> record){
		Graph_Node it = get_nth_node(i);
		vector<string> possible_vals = it.get_values();
		vector<float> prob_values(possible_vals.size(), -1);
		
		float sum = 0;
		for (int j = 0; j<possible_vals.size(); j++){
			prob_values[j] = markovBlanket(i,possible_vals[j],record);
			// cout << prob_values[j] << ":" << possible_vals[j] << endl;
			sum = sum + prob_values[j];
			//cumu_values[j] = sum;
		}
		float maxim = 0;
		int k=0;
		for (int j = 0; j<possible_vals.size(); j++){
			prob_values[j] = prob_values[j]/sum;
			if(prob_values[j]>maxim){
				maxim = prob_values[j];
				k = j;
			}
			//cumu_values[j] = cumu_values[j]/sum;
			// cout << cumu_values[j] << " ";
		}
		
		// vector<float> cumu_values(possible_vals.size(), -1);
		// float sum = 0;
		// for (int j = 0; j<possible_vals.size(); j++){
		// 	prob_values[j] = markovBlanket(i,possible_vals[j],record);
		// 	//cout << prob_values[j] << ":" << possible_vals[j] << endl;
		// 	sum = sum + prob_values[j];
		// 	cumu_values[j] = sum;
		// }
		// for (int j = 0; j<possible_vals.size(); j++){
		// 	prob_values[j] = prob_values[j]/sum;
		// 	cumu_values[j] = cumu_values[j]/sum;
		// 	// cout << cumu_values[j] << " ";
		// }


		// float roll_die = randRange(0,1);
		// int k = 0;
		// while (roll_die > cumu_values[k]){
		// 	k++;
		// }
		record[i] = possible_vals[k];
		return record;
	}

	// to convert values to parents to single number to fill in cpt
	int retVal(vector<int> Pval, vector<int> PvalSize){
		int n = Pval.size();
		int ret=0;
		int base=1;
		for(int i=n-1; i>-1; i--){
			ret += base*Pval[i];
			base = base * PvalSize[i];
		}
		return ret;
	}

	int index(string value, Graph_Node node){
		vector<string> val = node.get_values();

		for(int i=0; i<val.size(); i++){
			if(val[i]==value)
				return i;
		}

	}

	float smoothingCoeff(int n){
		// float f = ((sqrt((float)n)));
		float f = 1/sqrt((float)n);
		f = f/1000;
		return f;
	}

	void updateCPT(vector<vector<vector<string> > > &records){
		vector<Graph_Node>::iterator listIt;
		for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
		{
			int nodeNum = string_to_idx[listIt->get_name()];
			int nVal = listIt->get_nvalues();
			vector<string> parentsT = listIt->get_Parents();
			vector<int> parents;
			vector<int> PvalSize;
			int n = parentsT.size();
			int combinations=1;

			for(int i=0; i<n; i++){
				parents.push_back(string_to_idx[parentsT[i]]);
				int temp = get_nth_node(parents[i]).get_nvalues();
				PvalSize.push_back(temp);
				combinations = combinations * temp;
			}

			vector<float> cptNew;

			vector<vector<int> > storeCount(nVal, vector<int>(combinations, 0));

			for(int j=0; j<38; j++){
				for(int k=0; k<records[j].size(); k++){
					vector<int> v;
					int value = 0; // value number of the particular node
					string x = records[j][k][nodeNum];
					for(int l=0; l<n; l++){
						string s = records[j][k][parents[l]];
						v.push_back(index(s, get_nth_node(parents[l])));
					}	
					value = retVal(v, PvalSize);
					int varVal = index(x, *listIt);
					storeCount[varVal][value]++;
				}
			}

			vector<int> totalCount (combinations, 0);

			for(int i=0; i<combinations; i++){
				for(int j=0; j<nVal; j++){
					totalCount[i] += storeCount[j][i];
				}
			}
	
			for(int i=0; i<nVal; i++){
				for(int j=0; j<combinations; j++){
					if(totalCount[j]==0)
						totalCount[j]=1;
					// float f = ((float)storeCount[i][j] + (10/(float)totalCount[j]))/((float)totalCount[j] + ((float)nVal*10/(float)totalCount[j]));
					float sCoef = smoothingCoeff(totalCount[j]);
					float f = ((float)storeCount[i][j] + sCoef)/((float)totalCount[j] + ((float)nVal*sCoef));
					if(f<0.0001)
						f = 0.0001;
					cptNew.push_back(f);
				}
			}
			listIt->set_CPT(cptNew);
		}
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
		int count = 0;
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
					temp = temp.substr(1,temp.length()-2);
					values.push_back(temp);
					ss2>>temp;
				}
				name = name.substr(1,name.length()-2);
				string_to_idx[name] = count;
				idx_to_string[count] = name;
				count++;
				Graph_Node new_node(name,values.size(),values);
				int pos=Alarm_new.addNode(new_node);
			}
			else if(temp.compare("probability")==0)
			{
				ss>>temp;
				ss>>temp;
				temp = temp.substr(1,temp.length()-2);
				vector<Graph_Node>::iterator listIt;
				vector<Graph_Node>::iterator listIt1;
				listIt=Alarm_new.search_node(temp);
				int index=string_to_idx[temp];
				ss>>temp;
				values.clear();
				while(temp.compare(")")!=0)
				{
					temp = temp.substr(1,temp.length()-2);
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
				temp = temp.substr(1,temp.length()-2);
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
	ios_base::sync_with_stdio(false);cin.tie(0);cout.tie(0);cout<< fixed << setprecision(4);
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


	//Initialisation of network
	Alarm.initialise();
	// Alarm.printNetwork();
	// Alarm.printGraph();

    clock_t start;
    clock_t end;
	float elapsed = 0;

	int ct=0;

	// Run till some condition is satisfied
	while (elapsed < 60){
		//E-step
		//Inference of each variable
        start = clock();
		vector<float> probs(values,0);
		for (int i = 0; i < values; i++){
			for (int j = 0; j < records[i].size(); j++){
				records[i][j] = Alarm.fill(i,records[i][j]);
			}
		}
		// cout << "Done E-Step" << endl;

		//M-step
		//Use counting to get actual prob values
		Alarm.updateCPT(records);

		// cout << "Done M-step" << endl;

		end = clock();
		start = end - start;
        elapsed += (float)start/CLOCKS_PER_SEC;
		ct++;

	}
	cout<<ct<<endl;

	// creating solved_alarm.bif

	ifstream file("alarm.bif");
	ofstream outfile("solved_alarm.bif");

	outfile<< fixed << setprecision(4);


	if (file.is_open()){
		int count = 0;
		while( !file.eof()){
			stringstream ss;
			string line;
			string temp;
			vector<string> rec;
			getline (file,line);
			ss.str(line);
			ss >> temp;

			if (temp != "probability"){
				// cout << temp;
				if (count != 0){
					outfile<<endl;
				}
				count++;
				outfile << line;
			}
			else{
				outfile << endl;
				outfile << line << endl;
				ss >> temp;
				ss >> temp;
				temp = temp.substr(1,temp.length()-2);
				vector<float> cpt = Alarm.get_nth_node(string_to_idx[temp]).get_CPT();
				outfile << "	table ";
				for (int i = 0; i < cpt.size(); i++){
					outfile << cpt[i] << " ";
				}
				outfile << ";" << endl;
				outfile << "}";
				getline(file,line);
				getline(file,line);
			}
		}
	}


}





