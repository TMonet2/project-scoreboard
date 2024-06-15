## 记分牌算法仿真器

#### 实验目的

模拟记分牌算法，实现指令的动态调度，加深对指令相关性的理解。



#### 记分牌指令动态调度算法原理

- 记分牌是一个集中控制部件，其功能是控制数据寄存器与处理部件之间的数据传送。在记分牌中保存有与各个处理部件相联系的寄存器中的数据装载情况。当一个处理部件所要求的数据都已就绪，记分牌允许处理部件开始执行。当执行完成后，处理部件通知记分牌释放相关资源。所以在记分牌中记录了数据寄存器和多个处理部件状态的变化情况，通过它来检测和消除或减少数据相关性，加快程序的执行速度。目的是再无资源竞争的前提下保持每一个时钟周期执行一条指令的速率。
- 思想方法：尽可能提早指令的执行。当一条指令暂停执行时，如果其他后继指令与暂停指令及已发射的指令无任何相关，则仍然可以发射，执行。



#### 测试用例

在instance目录中，包含含有各种冲突的测试用例，以验证算法的正确性和性能。通过Instr_Init()函数对输入进行处理，预处理instruction。通过check函数进行验证。



#### 运行

1. 编译程序：`g++ -o scoreboard_sim scoreboard.cpp`
2. 运行程序：`./scoreboard_sim`
3. 查看输出结果，模拟器将打印每个周期的状态。



#### 框架

>   下面图展示了整个程序的执行流程。

![image-process](./scoreboard/picture/image-process.png)



#### 技术路线

1. 设计数据结构

```
struct node  //指令结构 node 用于存储每条指令的详细信息
{
	string instrct;//指令字符串
	string op;//操作类型，如加载（LD）、加法（ADDD）等
	int busy;//指令当前占用的功能部件编号
	int fi;//目标寄存器
        int fj, fk;//源寄存器
        int qj, qk;//源操作数队列标识，用于跟踪操作数的来源
        int rj, rk;//标记源寄存器 fj 和 fk 的数据是否已经准备好
	int cycle;//指令完成所需的时钟周期数
	int time[4];//录指令在不同阶段的时间戳
};

vector<node> instruction;
vector<int> Busy;//记录当前占用功能部件的指令索引
vector<int> result;//记录写回结果的指令索引，用于跟踪寄存器的最新写入者
vector<vector<int> > wait;//等待寄存器结果的指令队列

vector<vector<int> > tmp_process;//完成当前流程的指令
vector<set<int> > process;//实现issue-decode-exe-wb 四个阶段的指令队列

map<string, int> End_cycle;//指令对应的时钟周期
map<string, int> Func;//指令对应的功能部件名称
```



- 2.初始化和分配空间

处理输入，并对数组分配相应的空间

```
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
```



- 3.程序的循环（核心部分）

**发射**：如果指令的一个功能单元空闲（功能部件状态的`busy`字段），且没有其它活动指令以同一寄存器为目标寄存器（目标寄存器状态是否为空），则记分牌向功能单元发射指令，并更新指令状态。该步骤能解决**结构冒险**和**WRW冒险。**

```
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
```



**读取操作数**：如果不存在检查活动指令写入源寄存器（`Rj`和`Rk`字段为yes），记分牌将该指令发送到功能部件（`Rj`和`Rk`字段标记为no）。该步骤能解决**RAW冒险**。

```
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
```



**执行**：功能单元开始执行对应操作，更新功能单元状态。当结果就绪后，通知记分牌。将 `qj` ，`qk` ， `rj` 和 `rk` 重置为0，表示源操作数已被读取，不再需要等待（这个地方更新不能放在Read阶段，不然可能会出现读后写冲突！！！）

```
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
```



**写结果**：当其它活动指令不需要读取当前计算结果即将写入的寄存器，写回寄存器，并清空该条指令在记分牌记录的状态，同时修改以该指令目标寄存器作为源操作数的指令的`Rj`和`Rk`字段。该步骤能解决**WAR冒险**。

```
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
```

然后每次大循环执行完毕之后，将结果Display一下，然后cycle++。



- 4.其他亮点：全局变量部分（也可以通过控制台进行修改）

```
//使用数组来定义不同的操作类型，如加载（LD）、加法（ADDD）、减法（SUBD）、除法（DIVD）和乘法（MULTD），使得操作类型的管理和扩展变得简单明了。
string a[] = { " " ,"LD", "ADDD",  "SUBD",  "DIVD", "MULTD" };//操作类型
// 提供了功能部件的直观命名，帮助用户快速理解每个功能部件的用途
string name[] = { " ","Integer", "Mult1", "Mult2", "Add", "Divide" };

//灵活的时钟周期配置，允许模拟器根据指令类型安排执行时间。
End_cycle[a[1]] = 1;
End_cycle[a[2]] = 2;
End_cycle[a[3]] = 2;
End_cycle[a[4]] = 40;
End_cycle[a[5]] = 10;

//功能部件分配策略，定义了每种操作类型应该使用的特定功能部件
Func[a[1]] = 1;
Func[a[2]] = 4;
Func[a[3]] = 4;
Func[a[4]] = 5;
```

