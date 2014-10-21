#include "Database.h"
using namespace std;

Database::Database(){
}

Database::~Database(){
	for(int i=0; i<m_fieldIndex.size(); i++)
		m_fieldIndex[i]->clear();
}

bool Database::specifySchema(const std::vector<FieldDescriptor>& schema){
	//first reset the database
	while(!m_schema.empty()){
		m_schema.pop_back();
	}
	for(int i=m_rows.size()-1; !m_rows.empty(); i--){
		while(!m_rows[i].empty()){
			m_rows[i].pop_back();
		}
		m_rows.pop_back();
	}
	for(int i=m_fieldIndex.size()-1; !m_fieldIndex.empty(); i--){
		m_fieldIndex[i]->clear();	//clear the map
		m_fieldIndex.pop_back();
	}

	//check if at least one indexed field
	bool atLeastOne = false;
	for(int i=0; i<schema.size(); i++){
		if(schema[i].index == it_indexed){
			atLeastOne = true;
			break;
		}
	}
	if(atLeastOne == false)
		return false;		//no indexed field, immediately return false
	
	for(int i=0; i<schema.size(); i++){
		FieldDescriptor fd;
		fd.name = schema[i].name;
		fd.index = schema[i].index;
		m_schema.push_back(fd);
		m_fieldIndex.push_back(new MultiMap);
	}
	//initialize multi maps?
	return true;
}

bool Database::addRow(const std::vector<std::string>& rowOfData){
	//check if it's a valid row
	if(rowOfData.size() != m_schema.size())
		return false;

	m_rows.push_back(rowOfData);
	int n = m_rows.size()-1;
	
	for(int i=0; i<m_schema.size(); i++){
		if(m_schema[i].index == it_indexed)
			m_fieldIndex[i]->insert(rowOfData[i], n);
	}
	return true;
}

int Database::getNumRows() const{
	return m_rows.size();
}

bool Database::getRow(int rowNum, std::vector<std::string>& row) const{
	if(rowNum >= m_rows.size())
		return false;
	//reset row parameter
	while(!row.empty())
		row.pop_back();

	for(int i=0; i<m_schema.size(); i++){
		row.push_back(m_rows[rowNum][i]);
	}
	return true;
}

bool Database::loadFromURL(std::string url){
	string text;
	if(!HTTP().get(url, text)){
		cerr<< "Error fetching content from URL " << url << endl;
		return false;
	}
	else{

		//specify schema
		//string text = url;
		vector<FieldDescriptor> schema;
		string delimiters1 = "\n";
		Tokenizer l(text, delimiters1);
		string line;
		string field;
		while(l.getNextToken(line)){
			string delimiters2 = ",";
			Tokenizer f(line, delimiters2);
			while(f.getNextToken(field)){
				if(field[field.size()-1] == '*'){
					field = field.substr(0, field.size()-1);
					//cerr<<field<<endl;
					FieldDescriptor fd;
					fd.name = field;
					fd.index = it_indexed;
					schema.push_back(fd);
				}
				else{
					//cerr<<field<<endl;
					FieldDescriptor fd;
					fd.name = field;
					fd.index = it_none;
					schema.push_back(fd);
				}
			}
			break;
		}
		if(!specifySchema(schema))
			return false;

		//add rows
		vector<string> row;
		while(l.getNextToken(line)){
			string delimiters2 = ",";
			Tokenizer f(line, delimiters2);
			while(f.getNextToken(field)){
				//cerr<<field<<endl;
				row.push_back(field);
			}
			if(row.size() != m_schema.size())
				return false;
			addRow(row);
			while(!row.empty())
				row.pop_back();
		}
		return true;
	}
}

bool Database::loadFromFile(std::string filename){
	ifstream infile(filename);    
	if ( ! infile )		        // Did opening the file fail?
	{
	    cerr << "Error: Cannot open"<< filename << endl;
	}
	string text;
	string s;
	while (getline(infile, s))
	{
	    text += s;
		text += "\n";
	}
	//cerr<< text <<endl;
	//return true;

	//specify schema
	//string text = filename;
	vector<FieldDescriptor> schema;
	string delimiters1 = "\n";
	Tokenizer l(text, delimiters1);
	string line;
	string field;
	while(l.getNextToken(line)){
		string delimiters2 = ",";
		Tokenizer f(line, delimiters2);
		while(f.getNextToken(field)){
			if(field[field.size()-1] == '*'){
				//cerr<<field<<endl;
				field = field.substr(0, field.size()-1);
				FieldDescriptor fd;
				fd.name = field;
				fd.index = it_indexed;
				schema.push_back(fd);
			}
			else{
				//cerr<<field<<endl;
				FieldDescriptor fd;
				fd.name = field;
				fd.index = it_none;
				schema.push_back(fd);
			}
		}
		break;
	}
	if(!specifySchema(schema))
		return false;

	//add rows
	vector<string> row;
	while(l.getNextToken(line)){
		string delimiters2 = ",";
		Tokenizer f(line, delimiters2);
		while(f.getNextToken(field)){
			//cerr<<field<<endl;
			row.push_back(field);
		}
		if(row.size() != m_schema.size())
			return false;
		addRow(row);
		while(!row.empty())
			row.pop_back();
	}
	return true;
}

bool Database::rightOrder(int a, int b,
	const std::vector<SortCriterion>& sortCriteria)
{
	//cerr<<"checking order"<<endl;
	for(int j=0; j<sortCriteria.size(); j++){
		int index;	//find which field to look at
		for(int i=0; i<m_schema.size(); i++){
			if(sortCriteria[j].fieldName == m_schema[i].name){
				index = i;
				break;
			}
		}
		
		if(sortCriteria[j].ordering == ot_ascending){
			//cerr<<"asc"<<endl;
			//string ss1 = m_rows[a][index];
			//string ss2 = m_rows[b][index];
			if(m_rows[a][index] < m_rows[b][index])
				return true;
			if(m_rows[a][index] > m_rows[b][index])
				return false;
			continue;	//need to look at next criterion
		}
		else if(sortCriteria[j].ordering == ot_descending){
			//cerr<<"dsc"<<endl;
			if(m_rows[a][index] > m_rows[b][index])
				return true;
			if(m_rows[a][index] < m_rows[b][index])
				return false;
			continue;	//need to look at next criterion
		}
	}
	return true;	//tie, can't do anything
}

int Database::Partition(std::vector<int>& v, int 
		low, int high, const std::vector<SortCriterion>& sortCriteria){
	//cerr<<"partitioning"<<endl;
	int pi = low;	//no need to compare to pivot
	int pivot = v[low];
	do{
		while(low <= high && rightOrder(v[low], pivot, sortCriteria))
			low++;
		while(high>pi && rightOrder(pivot, v[high], sortCriteria)){
			//goes past 0, subscript range
			high--;
		}
		if(low < high){
			int temp = v[low];
			v[low] = v[high];
			v[high] = temp;
			//int vlow = v[low];
			//int vhigh = v[high];
			//int xxxx = 0;
		}
	}while (low < high);

	int temp = v[pi];
	v[pi] = v[high];
	v[high] = temp;
	pi = high;
	return pi;
}



void Database::QuickSort(std::vector<int>& v, int first, int last,
		const std::vector<SortCriterion>& sortCriteria){

	if(last - first >= 1){
		int pivotIndex;
		pivotIndex = Partition(v, first, last, sortCriteria);
		QuickSort(v, first, pivotIndex-1, sortCriteria);
		QuickSort(v, pivotIndex+1, last, sortCriteria);
	}
}

int Database::search(const std::vector<SearchCriterion>& searchCriteria,
					const std::vector<SortCriterion>& sortCriteria,
					std::vector<int>& results){
	if(searchCriteria.size() == 0)	return ERROR_RESULT;
	for(int i=0; i<searchCriteria.size(); i++){
		if(searchCriteria[i].maxValue == "" 
			&& searchCriteria[i].minValue == "")
			return ERROR_RESULT;
		bool valid = false;
		for(int j=0; j<m_schema.size(); j++){
			//string nn = searchCriteria[i].fieldName;
			//string mm = m_schema[j].name;
			if(searchCriteria[i].fieldName == m_schema[j].name)
				valid = true;
		}
		if(!valid)
			return ERROR_RESULT;
	}

	//start search
	//find rows that match the first criterion, store them to results
	int mapIndex1 = 0;
	for(int i=0; i<m_schema.size(); i++){
		if(searchCriteria[0].fieldName == m_schema[i].name){
			mapIndex1 = i;
			break;
		}
	}
	MultiMap::Iterator min1, max1;
	min1 = m_fieldIndex[mapIndex1]->findEqualOrSuccessor(searchCriteria[0].minValue);
	if(searchCriteria[0].maxValue == ""){ //get the very last 
		max1 = m_fieldIndex[mapIndex1]->findEqualOrPredecessor("zzzz");
		MultiMap::Iterator maxtemp1 = max1;
		while(maxtemp1.valid() && maxtemp1.next())
			max1.next();			//max indicates the very last node
	}
	else
		max1 = m_fieldIndex[mapIndex1]->findEqualOrPredecessor(searchCriteria[0].maxValue);
	while(min1.valid() && max1.valid() && min1.getKey() <= max1.getKey())	// && min1.getValue() != max1.getValue())
	{
		results.push_back(min1.getValue());
		//cerr<<results.size()<< ": " <<min1.getValue()<<endl;
		min1.next();
	}

	//Starting with second criterion,
	//find matching rows, store them into an unordered set,
	//traverse the results vector, and if an item is not in the unordered set,
	//get rid of the item. Repeat this for all the criteria

	vector<unordered_set<int> > matchSets;
	for(int j=1; j<searchCriteria.size(); j++){
		int mapIndex2 = 0;
		for(int i=0; i<m_schema.size(); i++){
			if(searchCriteria[j].fieldName == m_schema[i].name)
				mapIndex2 = i;
		}
		MultiMap::Iterator min2, max2;
		min2 = m_fieldIndex[mapIndex2]->findEqualOrSuccessor(searchCriteria[j].minValue);
		if(searchCriteria[j].maxValue == ""){ //get the very last 
			max2 = m_fieldIndex[mapIndex2]->findEqualOrPredecessor("zzzz");
			MultiMap::Iterator maxtemp = max2;
			while(maxtemp.valid() && maxtemp.next())
				max2.next();			//max indicates the very last node
		}
		else
			max2 = m_fieldIndex[mapIndex2]->findEqualOrPredecessor(searchCriteria[j].maxValue);
		unordered_set<int> tempset;
		while(min2.valid() && max2.valid() && min2.getKey() <= max2.getKey())	// && min2.getValue() != max2.getValue()){
		{
			tempset.insert(min2.getValue());
			//cerr<<tempset.size() << ": " << min2.getValue()<<endl;
			min2.next();
		}
		matchSets.push_back(tempset);
		//cerr<<"matchset"<<endl;

		//One crietrion has been procesed. From results get rid of items
		//that do not appear in the unordered set for this criterion.
		for(int k=0; k<results.size(); k++){
			if(matchSets[j-1].count(results[k]) == 0){
				results[k] = results[results.size()-1];	//replace with the last item
				results.pop_back();
				k--;	//reevaluate kth itme since it's been changed
			}
		}
	}
	//searching done. now sort results.

	QuickSort(results, 0, results.size()-1, sortCriteria);

	return results.size();
}







