#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include "MultiMap.h"
#include "Tokenizer.h"
#include "http.h"

class Database
{
public:
	enum IndexType { it_none, it_indexed };
	enum OrderingType { ot_ascending, ot_descending };

	struct FieldDescriptor
	{
		std::string name;
		IndexType index;
	};

	struct SearchCriterion
	{
		std::string fieldName;
		std::string minValue;
		std::string maxValue;
	};

	struct SortCriterion
	{
		std::string fieldName;
		OrderingType ordering;
	};

	static const int ERROR_RESULT = -1;

	Database();
	~Database();
	bool specifySchema(const std::vector<FieldDescriptor>& schema);
	bool addRow(const std::vector<std::string>& rowOfData);
	bool loadFromURL(std::string url);
	bool loadFromFile(std::string filename);
	int getNumRows() const;
	bool getRow(int rowNum, std::vector<std::string>& row) const;
	int search(const std::vector<SearchCriterion>& searchCriteria,
			const std::vector<SortCriterion>& sortCriteria,
			std::vector<int>& results);

private:
	//declare these private and don't implement them
	Database(const Database& other);
	Database& operator=(const Database& rhs);
	//bool compareRows(const std::vector<std::string> &a, const std::vector<std::string> &b);
	bool rightOrder(int a, int b,
					const std::vector<SortCriterion>& sortCriteria);
	int Partition(std::vector<int>& v, int low, int high,
					const std::vector<SortCriterion>& sortCriteria);
	void QuickSort(std::vector<int>& v, int first, int last,
					const std::vector<SortCriterion>& sortCriteria);

	std::vector<FieldDescriptor> m_schema;
	std::vector<std::vector<std::string> > m_rows;
	std::vector<MultiMap*> m_fieldIndex;
};
