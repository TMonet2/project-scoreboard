#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <iomanip> 
#include <cctype>
#include <fstream> 

using namespace std;

void test(string file1, string file2);
bool compareFiles(string file1, string file2);
string removeWhitespaceAndNewlines(string s);

// 函数用于去除字符串中的所有空白字符，包括空格和换行
string removeWhitespaceAndNewlines(string s) {
	string result;
	for (char c : s) {
		//忽略空格，非字母数字的字符，- 
		if(c == ' ' || !isalnum(c) || c == '-')continue;
		//如果是字母，则变成小写 
		if(isalpha(c))result.push_back(tolower(c));
		else result.push_back(c);
	}
	return result;
}

// 函数用于比较两个文件的内容
bool compareFiles(string file1, string file2) {
	ifstream f1(file1);
	ifstream f2(file2);

	if (!f1.is_open() || !f2.is_open()) {
		cerr << "Error opening one of the files." << endl;
		return false;
	}
	
	string s1, s2;
	string line1, line2;
	int cnt = 1;
	// 逐行比较文件内容
	while (getline(f1, line1) && getline(f2, line2)) {
		s1 = removeWhitespaceAndNewlines(line1);
		s2 = removeWhitespaceAndNewlines(line2);
		if(s1 != s2){
			//输出哪行不一样 
			cout << "Error in row :" << cnt << endl;
			cout<<cnt<<endl;
			return false;
		}
		cnt++;
	}
	
	// 检查是否两个文件都到达末尾，如果没有，说明文件长度不同
//	cout<<f1.eof()<<" "<<f2.eof()<<endl;
//	if(!f1.eof() || !f2.eof()) 
//	{
//		cout<<"error!!!";
//	}
	return true;
}

void test(string file1, string file2) {
	if (compareFiles(file1, file2)) 
	{
		cout << "The files are identical.Accept!" << endl;
	}
	else 
	{
		cout << "The files are different.Error!" << endl;
	}
}

int main()
{	
	//测试代码是否正确，比较结果和正确答案 
	test("./instance/output.txt","./instance/output1.txt");
	
	return 0;
} 
