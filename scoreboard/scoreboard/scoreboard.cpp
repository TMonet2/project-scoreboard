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

using namespace std;
string a[] = { " " ,"LD", "ADDD",  "SUBD",  "DIVD", "MULTD" };//操作类型
string name[] = { " ","Integer", "Mult1", "Mult2", "Add", "Divide" };
#define _CRT_SECURE_NO_WARNINGS
const int M = 30;//寄存器的个数
const int P = 5;//功能部件的个数
int n;//指令条数
struct node  //存放指令的参数
{
	string instrct;
	string op;
	int busy;
	int fi, fj, fk, rj, rk, qj, qk;
	int cycle;
	int time[4];
};
vector<node> instruction;
vector<int> Busy;//占用该功能部件的指令 
vector<int> result;//写该寄存器的指令 
vector<vector<int> > wait;//等待寄存器结果的指令 
vector<vector<int> > tmp_process;//完成当前流程的指令
vector<set<int> > process;//各个阶段的指令队列 
map<string, int> End_cycle;//每种指令对应的时钟周期
map<string, int> Func;//指令对应的功能部件名称
int x = 1;//发射阶段对应当前的下标
int cycle = 1;//对应当前的周期

void init();//初始化
void Instr_Init();//预处理输入
void issue_operation();//发射阶段
void read_operands();//读操作数阶段 
void execute();//执行阶段 
void write_result();//写结果阶段
string print_Space(int num);//输出空格
void print(int x);//输出空格


void init()//初始化赋值
{
	End_cycle[a[1]] = 1;
	End_cycle[a[2]] = 2;
	End_cycle[a[3]] = 2;
	End_cycle[a[4]] = 40;
	End_cycle[a[5]] = 10;

	Func[a[1]] = 1;
	Func[a[2]] = 4;
	Func[a[3]] = 4;
	Func[a[4]] = 5;
	Busy.resize(P + 1);//设置数组的空间
	result.resize(M + 1);
	wait.resize(M + 1);
	tmp_process.resize(4 + 1);//有4个阶段
	process.resize(4 + 2);//有4个阶段
}

void Instr_Init()
{
	int i, j, tmp;
	string s;
	bool flag;
	cin >> n;
	instruction.resize(n + 1);//初始化分配地址

	getchar();//清除输入流中的换行符，以避免影响后续的输入

	for (i = 1; i <= n; i++)
	{
		getline(cin, s);//字符具有空格，用getline整行读入
		if (s[0] == 'L')//初始化
		{
			instruction[i].op = a[1];
			instruction[i].cycle = 1;
		}
		else if (s[0] == 'A')
		{
			instruction[i].op = a[2];
			instruction[i].cycle = 2;
		}
		else if (s[0] == 'S')
		{
			instruction[i].op = a[3];
			instruction[i].cycle = 2;
		}
		else if (s[0] == 'D')
		{
			instruction[i].op = a[4];
			instruction[i].cycle = 40;
		}
		else if (s[0] == 'M')
		{
			instruction[i].op = a[5];
			instruction[i].cycle = 10;
		}

		flag = false;
		for (j = 1; j < s.size(); j++)
		{
			if (s[j - 1] == 'F')
			{
				flag = true;
				tmp = 0;
			}
			if (flag)
			{
				if (s[j] >= '0' && s[j] <= '9')
				{
					tmp = tmp * 10 + s[j] - '0';
				}
				else break;
			}
		}
		instruction[i].fi = tmp;

		if (s[0] == 'L')
		{
			flag = false;
			for (j; j < s.size(); j++)
			{
				if (s[j - 1] == 'R')
				{
					flag = true;
					tmp = 0;
				}
				if (flag)
				{
					if (s[j] >= '0' && s[j] <= '9')
					{
						tmp = tmp * 10 + s[j] - '0';
					}
					else break;
				}
			}
			instruction[i].fk = tmp;
		}
		else
		{
			flag = false;
			for (j; j < s.size(); j++)
			{
				if (s[j - 1] == 'F')
				{
					flag = true;
					tmp = 0;
				}
				if (flag)
				{
					if (s[j] >= '0' && s[j] <= '9')
					{
						tmp = tmp * 10 + s[j] - '0';
					}
					else break;
				}
			}
			instruction[i].fj = tmp;

			flag = false;
			for (j; j < s.size(); j++)
			{
				if (s[j - 1] == 'F')
				{
					flag = true;
					tmp = 0;
				}
				if (flag)
				{
					if (s[j] >= '0' && s[j] <= '9')
					{
						tmp = tmp * 10 + s[j] - '0';
					}
					else break;
				}
			}
			instruction[i].fk = tmp;
		}
		instruction[i].instrct = s;
	}
}

string print_Space(int num)
{
	string s = "";
	int i;
	for (i = 1; i <= num; i++)
	{
		s += " ";
	}
	return s;
}

void print()
{
	int i;
	node t;

	cout << "周期:" << cycle << endl;
	cout << "指令状态表:" << endl;
	cout << print_Space(20) << "|Issue|Read|Execution|Write|" << endl;
	for (i = 1; i <= n; i++)
	{
		t = instruction[i];
		cout << instruction[i].instrct << print_Space(20 - t.instrct.size()) << "|";
		if (t.time[0] != 0)printf("%-5d|", t.time[0]);
		else cout << print_Space(5) << "|";
		if (t.time[1] != 0)printf("%-4d|", t.time[1]);
		else cout << print_Space(4) << "|";
		if (t.time[2] != 0)printf("%-9d|", t.time[2]);
		else cout << print_Space(9) << "|";
		if (t.time[3] != 0)printf("%-5d|", t.time[3]);
		else cout << print_Space(5) << "|";
		cout << endl;
	}

	cout << endl;
	//输出功能部件状态表 
	cout << "功能部件状态:" << endl;
	cout << print_Space(10) << "|Busy|Op      |Fi  |Fj  |Fk  |Qj      |Qk      |Rj  |Rk  |" << endl;
	for (i = 1; i <= 5; i++)
	{
		cout << name[i] << print_Space(10 - name[i].size()) << "|";

		if (Busy[i] != 0)
		{
			t = instruction[Busy[i]];//当前占用这个功能部件的指令
			cout << "yes |";
			cout << t.op << print_Space(8 - t.op.size()) << "|";
			printf("F%-3d|", t.fi);

			if (t.op == a[1])
			{
				cout << print_Space(4) << "|";
				printf("R%-3d|", t.fk);
				cout << print_Space(8) << "|" << print_Space(8) << "|";

				if (t.time[1] != 0 && t.time[1] != x)cout << print_Space(4) << "|" << "no  |";
				else cout << print_Space(4) << "|" << "yes |";
			}
			else
			{
				printf("F%-3d|", t.fj);
				printf("F%-3d|", t.fk);

				if (t.rj == 0 && (t.time[1] == 0 || t.time[1] == x))
				{
					cout << name[instruction[result[t.fj]].busy] << print_Space(8 - name[instruction[result[t.fj]].busy].size()) << "|";
				}
				else cout << print_Space(8) << "|";
				if (t.rk == 0 && (t.time[1] == 0 || t.time[1] == x))
				{
					cout << name[instruction[result[t.fk]].busy] << print_Space(8 - name[instruction[result[t.fk]].busy].size()) << "|";
				}
				else cout << print_Space(8) << "|";
				if (t.rj == 0)cout << "no  |";
				else cout << "yes |";
				if (t.rk == 0)cout << "no  |";
				else cout << "yes |";

			}
			cout << endl;
		}
		else
		{
			cout << "no  |" << print_Space(8) << "|" << print_Space(4) << "|" << print_Space(4) << "|" << print_Space(4) << "|" << print_Space(8) << "|" << print_Space(8) << "|" << print_Space(4) << "|" << print_Space(4) << "|" << endl;
		}
	}

	cout << endl;
	//输出结果寄存器表 
	cout << "结果寄存器:" << endl;
	cout << print_Space(5) << "|";
	for (i = 0; i <= 30; i++)
	{
		printf("F%02d|", i);
	}
	cout << endl;

	cout << "item |";
	for (i = 0; i <= 30; i++)
	{
		t = instruction[result[i]];
		if (t.busy != 0)
		{
			if (t.busy == 2)
			{
				cout << name[t.busy].substr(0, 2) << "1|";
			}
			else if (t.busy == 3)
			{
				cout << name[t.busy].substr(0, 2) << "2|";
			}
			else
			{
				cout << name[t.busy].substr(0, 3) << "|";
			}

		}
		else
		{
			cout << print_Space(3) << "|";
		}
	}
	cout << endl;
}

void issue_operation()
{
	int fu;//前指令要使用的功能单元
	if (x <= n)
	{
		//判断功能部件是否有空闲
		bool if_busy = false;
		if (instruction[x].op == a[5] && (!Busy[2] || !Busy[3]))
		{
			if_busy = true;
		}
		else {
			if (!Busy[Func[instruction[x].op]])
			{
				if_busy = true;
			}
		}

		//判断是否有写后写（WAW）冲突
		if (if_busy && !result[instruction[x].fi])
		{
			if (instruction[x].op == a[5])
			{
				if (Busy[2] == 0)
				{
					instruction[x].busy = 2;
				}
				else if (Busy[3] == 0)
				{
					instruction[x].busy = 3;
				}
			}
			else if (!Busy[Func[instruction[x].op]])
			{
				instruction[x].busy = Func[instruction[x].op];
			}
			//修改记分牌内容
			fu = instruction[x].busy;
			Busy[fu] = x;

			instruction[x].qj = instruction[result[instruction[x].fj]].busy;
			instruction[x].qk = instruction[result[instruction[x].fk]].busy;
			if (result[instruction[x].fj] == 0)
			{
				instruction[x].rj = 1;
			}
			else
			{
				instruction[x].rj = 0;
				wait[instruction[x].fj].push_back(x);
			}
			if (result[instruction[x].fk] == 0)
			{
				instruction[x].rk = 1;
			}
			else
			{
				instruction[x].rk = 0;
				wait[instruction[x].fk].push_back(x);//等待寄存器的结果
			}
			result[instruction[x].fi] = x;
			instruction[x].time[0] = cycle;
			tmp_process[1].push_back(x);
			x++;
		}
	}
}

void read_operands()
{
	for (int x : process[2])
	{
		//判断源操作数是否准备就绪 
		if (instruction[x].rj && instruction[x].rk)
		{
			//修改记分牌内容
			instruction[x].time[1] = cycle;
			tmp_process[2].push_back(x);
		}
	}
}

void execute()
{
	for (int x : process[3])
	{
		//修改记分牌内容
		instruction[x].qj = 0;
		instruction[x].qk = 0;
		instruction[x].rj = 0;
		instruction[x].rk = 0;
		instruction[x].cycle--;
		if (instruction[x].cycle == 0)
		{
			instruction[x].time[2] = cycle;
			tmp_process[3].push_back(x);
		}
	}
}

void write_result()
{
	int i, j, tmp;
	bool flag;
	for (int x : process[4])
	{
		flag = false;
		//判断是否存在读后写（WAR）冲突
		for (int y : process[2])
		{
			if (y >= x) break;
			if ((instruction[y].fj != instruction[x].fi || instruction[y].rj == 0) && (instruction[y].fk != instruction[x].fi || instruction[y].rk == 0))
			{
			}
			else
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			instruction[x].time[3] = cycle;
			result[instruction[x].fi] = 0;
			Busy[instruction[x].busy] = 0;
			instruction[x].busy = 0;
			//将以指令x的母的寄存器为源操作数的指令的rj或者rk置为yes

			for (j = 0; j < wait[instruction[x].fi].size(); j++)
			{
				int y = wait[instruction[x].fi][j];
				if (instruction[y].fj == instruction[x].fi) instruction[y].rj = 1;
				if (instruction[y].fk == instruction[x].fi) instruction[y].rk = 1;
			}
			wait[instruction[x].fi].clear();

			tmp_process[4].push_back(x);
		}
	}
	for (i = 1; i <= 4; i++)
	{
		for (j = 0; j < tmp_process[i].size(); j++)
		{
			tmp = tmp_process[i][j];
			process[i].erase(tmp);
			process[i + 1].insert(tmp);

		}
		tmp_process[i].clear();//清空
	}
}


int main()
{
	//freopen("input.txt", "r", stdin);//从input.txt文件中读入
	//freopen("output.txt", "w", stdout);//输出到output.txt文件中
	init();
	Instr_Init();
	//算法正式开始
	while (true)
	{
		//如果所有指令均已完成
		if (process[5].size() == n) break;
		cout << "--------------------------------------------------------------------------------------------------------------------" << endl;
		issue_operation();
		read_operands();
		execute();
		write_result();
		print();
		cycle++;
		cout << "--------------------------------------------------------------------------------------------------------------------" << endl << endl;
	}
}

