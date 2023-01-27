#define _CRT_SECURE_NO_WARNINGS

/*
 * PL/0 编译器
 *
 * 使用方法：
 * 运行后输入PL/0源程序文件?
 * 回答是否输出虚拟机代码
 * 回答是否输出名字表
 * fa.pcode输出虚拟机代码
 * fa1.tmp输出源文件及其各行对应的首地址
 * out.tmp输出运行结果
 * fas.tmp输出名字表
 */

#include "pl0.h"
#include <string.h>
#include <windows.h>

 //运行前初始化，对保留字表 (word)、保留字表中每一个保留字对应的 symbol 类型 ( wsym )、
 //部分符号对应的 symbol 类型表 ( ssym )、类 PCODE 指令助记符表 ( mnemonic )、
 //声明开始集合 ( declbegsys )、表达式开始集合 ( statbegsys )、
 //项开始符号集合 ( facbegsys ) 以及一些全局变量的初始化
void init()
{
	jp.CycleNum = 0;
	jp.Addr = 0;
	int i;
	// ASCII 范围(0–31 控制字符, 32–126 分配给了能在键盘上找到的字符
	//数字 127 代表 DELETE 命令, 后 128 个是扩展 ASCII 打印字符) 因此共 256 个
	/* 设置单字符符号 */
	for (i = 0; i <= 255; i++)
	{
		ssym[i] = nul;
	}
	ssym['+'] = plus;
	ssym['-'] = minus;
	ssym['*'] = times;
	ssym['/'] = slash;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	ssym['='] = eql;
	ssym[','] = comma;
	ssym['.'] = period;
	ssym['#'] = neq;
	ssym[';'] = semicolon;
	ssym['['] = Lsquare;
	ssym[']'] = Rsquare;


	/* 设置保留字名字,按照字母顺序，便于折半查找 */
	strcpy(&(word[0][0]), "begin");

	strcpy(&(word[1][0]), "break");

	strcpy(&(word[2][0]), "call");
	strcpy(&(word[3][0]), "const");
	strcpy(&(word[4][0]), "do");
	strcpy(&(word[5][0]), "end");

	strcpy(&(word[6][0]), "for");

	strcpy(&(word[7][0]), "if");
	strcpy(&(word[8][0]), "odd");
	strcpy(&(word[9][0]), "procedure");
	strcpy(&(word[10][0]), "read");

	strcpy(&(word[11][0]), "step");

	strcpy(&(word[12][0]), "then");

	strcpy(&(word[13][0]), "until");

	strcpy(&(word[14][0]), "var");
	strcpy(&(word[15][0]), "while");
	strcpy(&(word[16][0]), "write");

	/* 设置保留字符号 */
	wsym[0] = beginsym;

	wsym[1] = breaksym;

	wsym[2] = callsym;
	wsym[3] = constsym;
	wsym[4] = dosym;
	wsym[5] = endsym;

	wsym[6] = forsym;

	wsym[7] = ifsym;
	wsym[8] = oddsym;
	wsym[9] = procsym;
	wsym[10] = readsym;

	wsym[11] = stepsym;

	wsym[12] = thensym;

	wsym[13] = untilsym;

	wsym[14] = varsym;
	wsym[15] = whilesym;
	wsym[16] = writesym;

	/* 设置指令名称 */
	strcpy(&(mnemonic[lit][0]), "lit");
	strcpy(&(mnemonic[opr][0]), "opr");
	strcpy(&(mnemonic[lod][0]), "lod");
	strcpy(&(mnemonic[sto][0]), "sto");
	strcpy(&(mnemonic[cal][0]), "cal");
	strcpy(&(mnemonic[inte][0]), "int");
	strcpy(&(mnemonic[jmp][0]), "jmp");
	strcpy(&(mnemonic[jpc][0]), "jpc");
	//数组
	strcpy(&(mnemonic[sta][0]), "sta");
	strcpy(&(mnemonic[lda][0]), "lda");

	/* 设置符号集 */
	for (i = 0; i < symnum; i++)
	{
		declbegsys[i] = false;
		statbegsys[i] = false;
		facbegsys[i] = false;
		ExpressionFollow[i] = false;
	}

	/* 设置声明开始符号集 */
	declbegsys[constsym] = true;
	declbegsys[varsym] = true;
	declbegsys[procsym] = true;

	/* 设置语句开始符号集 */
	statbegsys[beginsym] = true;
	statbegsys[callsym] = true;
	statbegsys[ifsym] = true;
	statbegsys[whilesym] = true;
	statbegsys[readsym] = true;
	statbegsys[writesym] = true;
	statbegsys[ident] = true;

	/* 设置因子开始符号集 */
	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[lparen] = true;

	/* 表达式后跟符号集 */
	ExpressionFollow[semicolon] = true;
	ExpressionFollow[period] = true;
	ExpressionFollow[comma] = true;
	ExpressionFollow[rparen] = true;
	ExpressionFollow[Rsquare] = true;
	ExpressionFollow[eql] = true;
	ExpressionFollow[neq] = true;
	ExpressionFollow[geq] = true;
	ExpressionFollow[leq] = true;
	ExpressionFollow[gtr] = true;
	ExpressionFollow[lss] = true;
	ExpressionFollow[endsym] = true;
	ExpressionFollow[thensym] = true;
	ExpressionFollow[dosym] = true;
	ExpressionFollow[stepsym] = true;
	ExpressionFollow[untilsym] = true;
}

//主函数，主要命令行询问
int main(int argc, char* argv[])
{
    SetConsoleOutputCP(65001);
	bool nxtlev[symnum];

	if (argc > 1) {
		strcpy(fname, argv[1]);
	}
	else
	{
		printf("请输入pl0文件");
		scanf("%s", &fname);
		//return 0;
	}

	fin = fopen(fname, "r");

	if (fin)
	{
		fa1 = fopen("fa1.tmp", "w");
		fprintf(fa1, "pl/0 file:   ");
		fprintf(fa1, "%s\n", fname);

		init();     /* 初始化 */

		err = 0;
		cc = cx = ll = 0;
		ch = ' ';

		//看能否取出第一个单词
		if (-1 != getsym())
		{
			fa = fopen("fa.pcode", "w");
			fas = fopen("fas.tmp", "w");
			addset(nxtlev, declbegsys, statbegsys, symnum);
			nxtlev[semicolon] = true;
			nxtlev[period] = true;
			if (-1 == block(0, 0, nxtlev))   /* 调用编译程序 */
			{
				fclose(fin);
				printf("\n");
			}
			fclose(fa);
			fclose(fas);
			//当前单词是否为.
			if (sym != period && err == 0)
			{
				error(9);
			}
			//要等所有错误处理完才能关闭错误打印文件
			fclose(fa1);
			//源程序没有错误
			if (err != 0) {
				printf("pl/0程序中存在错误！");
			}
			else {
				printf("恭喜，程序正确！\n编译完成(*^▽^*)\n");
				fa2 = fopen("out.tmp", "w");
				interpret();    /* 调用解释执行程序 */
				fclose(fa2);
			}
		}
		fclose(fin);
	}
	else
	{
		printf("Can't open file!\n");
	}

	printf("\n");
	system("pause");
	return 0;
}

#pragma region 词法分析


/*
* 漏掉换行符，读取一个字符。
*
* 读一行，存入line缓冲区，line被getsym取空后再读一行
*
* 每次从line缓冲区读出一个字符放在全局变量ch里面,输出代码行号
*
* 被函数getsym调用。
*/
int getch()
{
	//读完一行
	if (cc == ll)
	{
		if (feof(fin))  // 文件结束
		{
			printf("program incomplete");
			return -1;
		}
		ll = 0;
		cc = 0;
		printf("%d ", cx);
		fprintf(fa1, "%d ", cx);
		ch = ' ';
		//如果不是换行符
		while (ch != 10)
		{
			//读入一个字符到ch
			if (EOF == fscanf(fin, "%c", &ch))
			{
				line[ll] = 0;
				break;
			}
			printf("%c", ch);
			fprintf(fa1, "%c", ch);
			//往后再读一个字符
			line[ll] = ch;
			ll++;
		}
		printf("\n");
		fprintf(fa1, "\n");
	}
	ch = line[cc];
	cc++;
	return 0;
}

//词法分析，获取一个符号
//从源文件中读出若干有效字符，组成一个 token 串，识别它的类型为保留字/标识
//符/数字或是其它符号。如果是保留字，把 sym 置成相应的保留字类型，如果是标
//识符，把 sym 置成 ident 表示是标识符，于此同时，id 变量中存放的即为保留
//字字符串或标识符名字。如果是数字，把 sym 置为 number,同时 num 变量中存
//放该数字的值。如果是其它的操作符，则直接把 sym 置成相应类型。经过本过程后
//ch 变量中存放的是下一个即将被识别的字符

int getsym()
{
	int i, j, k;

	/* the original version lacks "\r", thanks to foolevery */
	while (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')  /* 忽略空格、换行、回车和TAB */
	{
		getchdo;
	}

	//名字或保留字（以A..z开头）
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
	{
		k = 0;
		do {
			if (k < al)
			{
				a[k] = ch;
				k++;
			}
			getchdo;
		} while (ch >= 'a' && ch <= 'z' || ch >= '0' && ch <= '9' || ch >= 'A' && ch <= 'Z');
		a[k] = 0;
		strcpy(id, a);
		i = 0;
		j = norw - 1;
		do {    /* 二分查找，搜索当前符号是否为保留字 */
			k = (i + j) / 2;
			if (strcmp(id, word[k]) <= 0)
			{
				j = k - 1;
			}
			if (strcmp(id, word[k]) >= 0)
			{
				i = k + 1;
			}
		} while (i <= j);
		if (i - 1 > j)
		{
			sym = wsym[k]; // 找到则标记保留字
		}
		else
		{
			sym = ident; // 否则标记为标识符
		}
	}
	// 为数字：以0..9开头 
	else if (ch >= '0' && ch <= '9')
	{
		k = 0;
		num = 0;
		sym = number;
		do {
			num = 10 * num + ch - '0';
			k++;
			getchdo;
		} while (ch >= '0' && ch <= '9'); /* 获取数字的值 */
		k--;
		if (k > nmax)
		{
			errorDo(30);
		}
	}
	//占两个字符的运算符及界符
	else
	{
		if (ch == ':')      /* 检测赋值符号 */
		{
			getchdo;
			if (ch == '=')
			{
				sym = becomes;
				getchdo;
			}
			else
			{
				sym = nul;  /* 不能识别的符号 */
			}
		}
		else if (ch == '<')      /* 检测小于或小于等于符号 */
		{
			getchdo;
			if (ch == '=')
			{
				sym = leq;
				getchdo;
			}
			else
			{
				sym = lss;
			}
		}
		else if (ch == '>')        /* 检测大于或大于等于符号 */
		{
			getchdo;
			if (ch == '=')
			{
				sym = geq;
				getchdo;
			}
			else
			{
				sym = gtr;
			}
		}
		else if (ch == '\'')        /* 检测单引号框起来的输出字符 */
		{
			sym = OutStr;
			int index = 0;
			getchdo;
			while (ch != '\'' && index < StrLen)
			{
				OutString[index] = ch;
				index++;
				getchdo;
			}
			if (ch != '\'' && index == StrLen) {
				errorDo(22);
			}
			getchdo;
			OutString[index] = 0;
		}
		else            /* 当符号不满足上述条件时，全部按照单字符符号处理 */
		{
			sym = ssym[ch];
			if (sym != period)
			{
				getchdo;
			}
		}
	}
	return 0;
}

#pragma endregion

#pragma region 语法分析

/*
* 编译程序主模块
*
* lev:    当前分程序所在层
* tx:     名字表当前尾指针
* fsys:   当前模块后跟符号集
*/
int block(int lev, int tx, bool* fsys)
{
	int i;

	int dx;                 /* 名字分配到的相对地址 */
	int tx0;                /* 保留初始tx */
	int cx0;                /* 保留初始cx */
	bool nxtlev[symnum];    /* 在下级函数的参数中，符号集合均为值参，但由于使用数组实现，
							传递进来的是指针，为防止下级函数改变上级函数的集合，开辟新的空间
							传递给下级函数*/

	dx = 3;
	tx0 = tx;               /* 记录本层符号的初始位置 */
	table[tx].adr = cx;

	gendo(jmp, 0, 0);

	if (lev > levmax)
	{
		errorDo(32);
	}

	do {

		if (sym == constsym)    /* 收到常量声明符号，开始处理常量声明 */
		{
			getsymdo;
			constdeclarationdo(&tx, lev, &dx);  /* dx的值会被constdeclaration改变，使用指针 */
			while (sym == comma)
			{
				getsymdo;
				constdeclarationdo(&tx, lev, &dx);
			}
			if (sym == semicolon)
			{
				getsymdo;
			}
			else
			{
				errorDo(5);   /*漏掉了逗号或者分号*/
			}
		}

		if (sym == varsym)      /* 收到变量声明符号，开始处理变量声明 */
		{
			getsymdo;

			vardeclarationdo(&tx, lev, &dx);
			while (sym == comma)
			{
				getsymdo;
				vardeclarationdo(&tx, lev, &dx);
			}
			if (sym == semicolon)
			{
				getsymdo;
			}
			else
			{
				errorDo(5);
			}
		}

		while (sym == procsym) /* 收到过程声明符号，开始处理过程声明 */
		{
			getsymdo;

			if (sym == ident)
			{
				enter(procedur, &tx, lev, &dx); /* 记录过程名字 */
				getsymdo;
			}
			else
			{
				errorDo(4);   /* procedure后应为标识符 */
			}

			if (sym == semicolon)
			{
				getsymdo;
			}
			else
			{
				errorDo(5);   /* 漏掉了分号 */
			}

			memcpy(nxtlev, fsys, sizeof(bool) * symnum);
			nxtlev[semicolon] = true;
			if (-1 == block(lev + 1, tx, nxtlev))
			{
				return -1;  /* 递归调用 */
			}

			if (sym == semicolon)
			{
				getsymdo;
				memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
				nxtlev[procsym] = true;
				testdo(nxtlev, fsys, 6);
			}
			else
			{
				errorDo(5);   /* 漏掉了分号 */
			}
		}
		memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
		testdo(nxtlev, declbegsys, 7);
	} while (inset(sym, declbegsys));   /* 直到没有声明符号 */

	code[table[tx0].adr].a = cx;    /* 开始生成当前过程代码 */
	table[tx0].adr = cx;            /* 当前过程代码地址 */
	table[tx0].size = dx;           /* 声明部分中每增加一条声明都会给dx增加1，声明部分已经结束，dx就是当前过程数据的size */
	cx0 = cx;
	gendo(inte, 0, dx);             /* 生成分配内存代码 */

	ListTable(tx0, tx);        //输出符号表

	/* 语句后跟符号为分号或end */
	memcpy(nxtlev, fsys, sizeof(bool) * symnum);  /* 每个后跟符号集和都包含上层后跟符号集和，以便补救 */
	nxtlev[semicolon] = true;
	nxtlev[endsym] = true;
	statementdo(nxtlev, &tx, lev);
	gendo(opr, 0, 0);                       /* 每个过程出口都要使用的释放数据段指令 */
	memset(nxtlev, 0, sizeof(bool) * symnum); /*分程序没有补救集合 */
	testdo(fsys, nxtlev, 8);                /* 检测后跟符号正确性 */
	listcode(cx0);                          /* 输出代码 */
	return 0;
}
/*
* 语句处理
*/
int statement(bool* fsys, int* ptx, int lev)
{
	int i, cx1, cx2, ArrayNo = -1;
	bool nxtlev[symnum];

	if (sym == ident)   /* 准备按照赋值语句处理 */
	{
		i = position(id, *ptx);
		if (i == 0)
		{
			errorDo(11);  /* 变量未找到 */
		}
		else
		{
			if (table[i].kind != variable && table[i].kind != array)
			{
				errorDo(12);  /* 赋值语句格式错误 */
				i = 0;
			}
			else
			{
				if (table[i].kind == array) {
					getsymdo;
					if (sym == Lsquare) { getsymdo; }
					else error(25);
					expressiondo(nxtlev, ptx, lev);//数组序号保存在栈顶
					if (sym != Rsquare) error(25);
				}
				getsymdo;
				if (sym == becomes)
				{
					getsymdo;
				}
				else
				{
					errorDo(13);  /* 没有检测到赋值符号 */
				}
				memcpy(nxtlev, fsys, sizeof(bool) * symnum);
				expressiondo(nxtlev, ptx, lev); /* 处理赋值符号右侧表达式 */
				if (i != 0)
				{
					/* expression将执行一系列指令，但最终结果将会保存在栈顶，执行sto命令完成赋值 */
					if (table[i].kind == array)
					{
						gendo(sta, lev - table[i].level, table[i].adr);
					}
					else { gendo(sto, lev - table[i].level, table[i].adr); }
				}
			}
		}//if (i == 0)
	}
	else if (sym == readsym) /* 准备按照read语句处理 */
	{
		getsymdo;
		if (sym != lparen)
		{
			errorDo(34);  /* 格式错误，应是左括号 */
		}
		else
		{
			do {
				getsymdo;
				if (sym == ident || sym == array)
				{
					i = position(id, *ptx); /* 查找要读的变量 */
				}
				else
				{
					i = 0;
				}

				if (i == 0)
				{
					errorDo(35);  /* read()中应是声明过的变量名 */
				}
				else if (table[i].kind != variable && table[i].kind != array)
				{
					errorDo(35);	/* read()参数表的标识符不是变量 */
				}
				else
				{
					if (table[i].kind == array) {
						getsymdo;
						if (sym == Lsquare) { getsymdo; }
						else errorDo(25);
						expressiondo(nxtlev, ptx, lev);
						if (sym != Rsquare) error(25);
					}
					gendo(opr, 0, 16);  /* 生成输入指令，读取值到栈顶 */
					if (table[i].kind == array)
					{
						gendo(sta, lev - table[i].level, table[i].adr);
					}
					else { gendo(sto, lev - table[i].level, table[i].adr);/* 储存到变量 */ }
				}
				getsymdo;

			} while (sym == comma); /* 一条read语句可读多个变量 */
		}
		if (sym != rparen)
		{
			errorDo(33);  /* 格式错误，应是右括号 */
			while (!inset(sym, fsys))   /* 出错补救，直到收到上层函数的后跟符号 */
			{
				getsymdo;
			}
		}
		else
		{
			getsymdo;
		}
	}
	else if (sym == writesym)    /* 准备按照write语句处理，与read类似 */
	{
		getsymdo;
		if (sym == lparen)
		{
			do {
				getsymdo;
				memcpy(nxtlev, fsys, sizeof(bool) * symnum);
				nxtlev[rparen] = true;
				nxtlev[comma] = true;       /* write的后跟符号为) or , */
				if (sym == OutStr)//符号串
				{
					int index = 0;
					while (OutString[index] != 0)
					{
						//将字符串的ASCII码放入栈顶，再按照字符格式输出
						gen(lit, 0, OutString[index]);
						gen(opr, 0, 17);
						index++;
					}
					getsymdo;
				}
				else //表达式
				{
					expressiondo(nxtlev, ptx, lev); /* 调用表达式处理，此处与read不同，read为给变量赋值 */
					gendo(opr, 0, 14);  /* 生成输出指令，输出栈顶的值 */
				}
			} while (sym == comma);
			if (sym != rparen)
			{
				errorDo(33);  /* write()中应为完整表达式 */
			}
			else
			{
				getsymdo;
			}
		}
		gendo(opr, 0, 15);  /* 输出换行 */
	}
	else if (sym == callsym) /* 准备按照call语句处理 */
	{
		getsymdo;
		if (sym != ident)
		{
			errorDo(14);  /* call后应为标识符 */
		}
		else
		{
			i = position(id, *ptx);
			if (i == 0)
			{
				errorDo(11);  /* 过程未找到 */
			}
			else
			{
				if (table[i].kind == procedur)
				{
					gendo(cal, lev - table[i].level, table[i].adr);   /* 生成call指令 */
				}
				else
				{
					errorDo(15);  /* call后标识符应为过程 */
				}
			}
			getsymdo;
		}
	}
	else if (sym == ifsym)   /* 准备按照if语句处理 */
	{
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[thensym] = true;
		nxtlev[dosym] = true;   /* 后跟符号为then或do */
		conditiondo(nxtlev, ptx, lev); /* 调用条件处理（逻辑运算）函数 */
		if (sym == thensym)
		{
			getsymdo;
		}
		else
		{
			errorDo(16);  /* 缺少then */
		}
		cx1 = cx;   /* 保存当前指令地址 */
		gendo(jpc, 0, 0);   /* 生成条件跳转指令，跳转地址未知，暂时写0 */
		statementdo(fsys, ptx, lev);    /* 处理then后的语句 */
		code[cx1].a = cx;   /* 经statement处理后，cx为then后语句执行完的位置，它正是前面未定的跳转地址 */
	}
	else if (sym == beginsym)    /* 准备按照复合语句处理 */
	{
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[semicolon] = true;
		nxtlev[endsym] = true;  /* 后跟符号为分号或end */
		/* 循环调用语句处理函数，直到下一个符号不是语句开始符号或收到end */
		statementdo(nxtlev, ptx, lev);

		while (inset(sym, statbegsys) || sym == semicolon)
		{
			if (sym == semicolon)
			{
				getsymdo;
			}
			else
			{
				errorDo(10);  /* 缺少分号 */
			}
			statementdo(nxtlev, ptx, lev);
		}
		if (sym == endsym)
		{
			getsymdo;
		}
		else
		{
			errorDo(17);  /* 缺少end或分号 */
		}
	}
	else if (sym == whilesym)    /* 准备按照while语句处理 */
	{
		jp.CycleNum += 1;
		cx1 = cx;   /* 保存判断条件操作的位置 */
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[dosym] = true;   /* 后跟符号为do */
		conditiondo(nxtlev, ptx, lev);  /* 调用条件处理 */
		cx2 = cx;   /* 保存jpc指令的位置 */
		gendo(jpc, 0, 0);   /* 生成条件跳转，但跳出循环的地址未知 */
		if (sym == dosym)
		{
			getsymdo;
		}
		else
		{
			errorDo(18);  /* 缺少do */
		}
		statementdo(fsys, ptx, lev);    /* 循环体 */
		gendo(jmp, 0, cx1); /* 回头重新判断条件 */
		code[cx2].a = cx;   /* 反填跳出循环的地址，与if类似 */
		//若有break或continue语句
		if (jp.Addr != 0) {
			code[jp.Addr].a = cx;   //反填跳出循环的地址
			jp.Addr = 0;
		}
		jp.CycleNum -= 1;
	}
	else if (sym == forsym)    /* 准备按照for语句处理 */
	{
		jp.CycleNum += 1;

		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[dosym] = true;   /* 后跟符号为do */
		i = position(id, *ptx); //存储标识符在符号表中的位置
		statementdo(nxtlev, ptx, lev);
		cx1 = cx;   /* 保存判断条件操作的位置 */
		if (sym == stepsym) {
			getsymdo;
			expressiondo(nxtlev, ptx, lev);//每次要增加的值存在栈顶，设为t
		}
		else
		{
			errorDo(28);
		}
		if (sym == untilsym) {
			getsymdo;
			expressiondo(nxtlev, ptx, lev);//停止值存在栈顶，设为t+1
		}
		else
		{
			errorDo(28);
		}
		//变量的值存在栈顶，设为t+2
		if (table[i].kind == variable) {
			gendo(lod, lev - table[i].level, table[i].adr);   /* 找到变量地址并将其值入栈 */
		}
		else
		{
			errorDo(29);
		}
		gendo(opr, 0, 9);//变量未达到设定停止值？
		cx2 = cx;   /* 保存jpc指令的位置 */
		gendo(jpc, 0, 0);   /* 生成条件跳转，但跳出循环的地址未知 */

		if (sym == dosym)
		{// 找到变量地址并将其值入栈   t+1
			gendo(lod, lev - table[i].level, table[i].adr);
			gendo(opr, 0, 2);//变量+每次要增加的值 t
			gendo(sto, lev - table[i].level, table[i].adr);
			getsymdo;
		}
		else
		{
			errorDo(18);  /* 缺少do */
		}

		statementdo(fsys, ptx, lev);    /* 循环体 */
		gendo(jmp, 0, cx1); /* 回头重新判断条件 */
		code[cx2].a = cx;   // 反填跳出循环的地址
		//若有break或continue语句
		if (jp.Addr != 0) {
			code[jp.Addr].a = cx;   //反填跳出循环的地址
			jp.Addr = 0;
		}
		jp.CycleNum -= 1;
	}
	else if (sym == breaksym)
	{
		getsymdo;
		if (jp.CycleNum == 0) {
			errorDo(27);
		}
		else
		{
			jp.Addr = cx;
			gendo(jmp, 0, 0); /* 跳出循环地址未知 */
		}
	}
	else
	{
		memset(nxtlev, 0, sizeof(bool) * symnum); /* 语句结束无补救集合 */
		testdo(fsys, nxtlev, 19);   /* 检测语句结束的正确性 */
	}

	return 0;
}
/*
* 常量声明处理
*/
int constdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		getsymdo;
		if (sym == eql || sym == becomes)
		{
			if (sym == becomes)
			{
				errorDo(1);   /* 把=写成了:= */
			}
			getsymdo;
			if (sym == number)
			{
				enter(constant, ptx, lev, pdx);
				getsymdo;
			}
			else
			{
				errorDo(2);   /* 常量说明=后应是数字 */
			}
		}
		else
		{
			errorDo(3);   /* 常量说明标识后应是= */
		}
	}
	else
	{
		error(4);   /* const后应是标识 */
	}
	return 0;
}

/*
* 变量声明处理
ptx为符号表尾位置
lev为当前层
pdx为为在当前层的偏移量
*/
int vardeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		getsymdo;
		//数组
		if (sym == Lsquare) {
			getsymdo;
			if (sym == number) {
				getsymdo;
				enter(array, ptx, lev, pdx);
			}
			else error(25);
			if (sym == Rsquare) {
				getsymdo;
			}
			else error(25);
		}
		else enter(variable, ptx, lev, pdx); // 填写名字表
	}
	else
	{
		errorDo(4);   /* var后应是标识 */
	}
	return 0;
}
/*
* 表达式处理
*/
int expression(bool* fsys, int* ptx, int lev)
{
	enum symbol addop;  /* 用于保存正负号 */
	bool nxtlev[symnum];

	if (sym == plus || sym == minus) /* 开头的正负号，此时当前表达式被看作一个正的或负的项 */
	{
		addop = sym;    /* 保存开头的正负号 */
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* 处理项 */
		if (addop == minus)
		{
			gendo(opr, 0, 1); /* 如果开头为负号生成取负指令 */
		}
	}
	else    /* 此时表达式被看作项的加减 */
	{
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* 处理项 */
	}
	while (sym == plus || sym == minus)
	{
		addop = sym;
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* 处理项 */
		if (addop == plus)
		{
			gendo(opr, 0, 2);   /* 生成加法指令 */
		}
		else
		{
			gendo(opr, 0, 3);   /* 生成减法指令 */
		}
	}
	return 0;
}

/*
* 项处理
*/
int term(bool* fsys, int* ptx, int lev)
{
	enum symbol mulop;  /* 用于保存乘除法符号 */
	bool nxtlev[symnum];

	memcpy(nxtlev, fsys, sizeof(bool) * symnum);
	nxtlev[times] = true;
	nxtlev[slash] = true;
	factordo(nxtlev, ptx, lev); /* 处理因子 */
	while (sym == times || sym == slash)
	{
		mulop = sym;
		getsymdo;
		factordo(nxtlev, ptx, lev);
		if (mulop == times)
		{
			gendo(opr, 0, 4);   /* 生成乘法指令 */
		}
		else
		{
			gendo(opr, 0, 5);   /* 生成除法指令 */
		}
	}
	return 0;
}

/*
* 因子处理
*/
int factor(bool* fsys, int* ptx, int lev)
{
	int i;
	bool nxtlev[symnum];
	testdo(facbegsys, fsys, 24);    /* 检测因子的开始符号 */
	if (inset(sym, facbegsys))    /* BUG: 原来的方法var1(var2+var3)会被错误识别为因子 */
	{
		if (sym == ident)    /* 因子为常量或变量 */
		{
			i = position(id, *ptx); /* 查找名字 */
			if (i == 0)
			{
				errorDo(11);  /* 标识符未声明 */
			}
			else
			{
				switch (table[i].kind)
				{
				case constant:  /* 名字为常量 */
					gendo(lit, 0, table[i].val);    /* 直接把常量的值入栈 */
					break;
				case variable:  /* 名字为变量 */
					gendo(lod, lev - table[i].level, table[i].adr);   /* 找到变量地址并将其值入栈 */
					break;
				case array:  /* 名字为数组 */
					getsymdo;
					if (sym == Lsquare) { getsymdo; }
					else error(25);
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					nxtlev[rparen] = true;
					expressiondo(nxtlev, ptx, lev);
					if (sym != Rsquare) error(25);
					gendo(lda, lev - table[i].level, table[i].adr);   /* 找到数组元素地址并将其值入栈 */
					break;
				case procedur:  /* 名字为过程 */
					errorDo(21);  /* 不能为过程 */
					break;
				}
			}
			getsymdo;
		}
		else if (sym == number)   /* 因子为数 */
		{
			if (num > amax)
			{
				errorDo(31);
				num = 0;
			}
			gendo(lit, 0, num);
			getsymdo;
		}
		else
		{
			if (sym == lparen)  /* 因子为表达式 */
			{
				getsymdo;
				memcpy(nxtlev, fsys, sizeof(bool) * symnum);
				nxtlev[rparen] = true;
				expressiondo(nxtlev, ptx, lev);
				if (sym == rparen)
				{
					getsymdo;
				}
				else
				{
					errorDo(33);  /* 缺少右括号 */
				}
			}
			testdo(fsys, facbegsys, 23);    /* 因子后有非法符号 */
		}
	}
	return 0;
}

/*
* 条件处理
*/
int condition(bool* fsys, int* ptx, int lev)
{
	enum symbol relop;
	bool nxtlev[symnum];

	if (sym == oddsym)   /* 准备按照odd运算处理 */
	{
		getsymdo;
		expressiondo(fsys, ptx, lev);
		gendo(opr, 0, 6);   /* 生成odd指令 */
	}
	else
	{
		/* 逻辑表达式处理 */
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[eql] = true;
		nxtlev[neq] = true;
		nxtlev[lss] = true;
		nxtlev[leq] = true;
		nxtlev[gtr] = true;
		nxtlev[geq] = true;
		expressiondo(nxtlev, ptx, lev);
		if (sym != eql && sym != neq && sym != lss && sym != leq && sym != gtr && sym != geq)
		{
			errorDo(20);
		}
		else
		{
			relop = sym;
			getsymdo;
			expressiondo(fsys, ptx, lev);
			switch (relop)
			{
			case eql:
				gendo(opr, 0, 8);
				break;
			case neq:
				gendo(opr, 0, 9);
				break;
			case lss:
				gendo(opr, 0, 10);
				break;
			case geq:
				gendo(opr, 0, 11);
				break;
			case gtr:
				gendo(opr, 0, 12);
				break;
			case leq:
				gendo(opr, 0, 13);
				break;
			}
		}
	}
	return 0;
}

#pragma endregion

#pragma region 中间代码生成
/*
* 生成虚拟机代码
*
* x: instruction.f;
* y: instruction.l;
* z: instruction.a;
*/
int gen(enum fct x, int y, int z)
{
	if (cx >= cxmax)
	{
		printf("程序过长！");
		return -1;
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx].a = z;
	cx++;
	return 0;
}


/*
* 输出目标代码清单
*/
void listcode(int cx0)
{
	int i;
	for (i = cx0; i < cx; i++)
	{
		printf("%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		fprintf(fa, "%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
}

#pragma endregion


#pragma region 解释器
/*
* 解释程序,仅执行一遍
*/
void interpret()
{
	int p, b, t;    /* 指令指针p PC，指令基址base，栈顶指针top */
	struct instruction i;   /* 存放当前指令 */
	int s[stacksize];   /* 栈 */

	printf("start pl0\n");
	t = 0;
	b = 0;
	p = 1;
	s[0] = s[1] = s[2] = 0;
	do {
		//printf("%d %s %d %d\n", p, mnemonic[code[p].f], code[p].l, code[p].a);

		i = code[p];
		p++;
		switch (i.f)
		{
		case lit:   /* 将a的值取到栈顶 */
			s[t] = i.a;
			t++;
			break;
		case opr:   /* 数学、逻辑运算 */
			switch (i.a)
			{
			case 0://过程返回
				t = b;
				p = s[t + 2];//RA
				b = s[t + 1];//DL
				break;
			case 1://求相反数
				s[t - 1] = -s[t - 1];
				break;
			case 2://次栈顶值与栈顶相加，存入次栈顶
				t--;
				s[t - 1] = s[t - 1] + s[t];
				break;
			case 3://次栈顶值-栈顶，存入次栈顶
				t--;
				s[t - 1] = s[t - 1] - s[t];
				break;
			case 4://次栈顶值*栈顶，存入次栈顶
				t--;
				s[t - 1] = s[t - 1] * s[t];
				break;
			case 5://次栈顶值/栈顶，存入次栈顶
				t--;
				s[t - 1] = s[t - 1] / s[t];
				break;
			case 6://栈顶元素求mod2，存入栈顶
				s[t - 1] = s[t - 1] % 2;
				break;
			case 8://次栈顶值==栈顶？，存入次栈顶
				t--;
				s[t - 1] = (s[t - 1] == s[t]);
				break;
			case 9://次栈顶值！=栈顶？，存入次栈顶
				t--;
				s[t - 1] = (s[t - 1] != s[t]);
				break;
			case 10://次栈顶值<栈顶？，存入次栈顶
				t--;
				s[t - 1] = (s[t - 1] < s[t]);
				break;
			case 11://次栈顶值>=栈顶？，存入次栈顶
				t--;
				s[t - 1] = (s[t - 1] >= s[t]);
				break;
			case 12://次栈顶值>栈顶？，存入次栈顶
				t--;
				s[t - 1] = (s[t - 1] > s[t]);
				break;
			case 13://次栈顶值<=栈顶？，存入次栈顶
				t--;
				s[t - 1] = (s[t - 1] <= s[t]);
				break;
			case 14://输出栈顶值，栈顶指针--
				printf("%d", s[t - 1]);
				fprintf(fa2, "%d", s[t - 1]);
				t--;
				break;
			case 15://输出\n
				printf("\n");
				fprintf(fa2, "\n");
				break;
			case 16://输出?，并从控制台读入一个数字到栈顶
				printf("?");
				fprintf(fa2, "?");
				scanf("%d", &(s[t]));
				fprintf(fa2, "%d\n", s[t]);
				t++;
				break;
			case 17://输出字符栈顶值，栈顶指针--
				printf("%c", s[t - 1]);
				fprintf(fa2, "%c", s[t - 1]);
				t--;
				break;
			}
			break;
		case lod:   /* 取相对当前过程层差为l，偏移量为A的单元的值到栈顶 */
			s[t] = s[base(i.l, s, b) + i.a];
			t++;
			break;
		case sto:   /* 栈顶的值存到相对当前过程层差为l，偏移量为A的单元 */
			t--;
			s[base(i.l, s, b) + i.a] = s[t];
			break;
		case lda:   /* 取相对当前过程层差为l，偏移量为（A+次栈顶）的单元的值到次栈顶 */
			s[t - 1] = s[base(i.l, s, b) + i.a + s[t - 1]];
			break;
		case sta:   /* 栈顶的值存到相对当前过程层差为l，偏移量为（A+次栈顶）的单元 */
			t--;
			s[base(i.l, s, b) + i.a + s[t - 1]] = s[t];
			t--;
			break;
		case cal:   /* 调用子过程 */
			s[t] = base(i.l, s, b); /* 将父过程基地址入栈，SL */
			s[t + 1] = b; /* DL，调用该过程的过程基地址入栈，此两项用于base函数 */
			s[t + 2] = p; /* RA，将当前指令指针入栈，用于之后过程完了返回 */
			b = t;  /* 改变基地址指针值为新过程的基地址 */
			p = i.a;    /* 跳转 */
			break;
		case inte:  /* 分配内存 */
			t += i.a;
			break;
		case jmp:   /* 直接跳转 */
			p = i.a;
			break;
		case jpc:   /* 条件跳转 */
			t--;
			if (s[t] == 0)
			{
				p = i.a;
			}
			break;
		}
	} while (p != 0);
}

/* 求出定义该过程的过程基址
*l为层次差，b为上一过程（调用该过程的过程）基址，s为栈
*/
int base(int l, int* s, int b)
{
	int b1;
	b1 = b;
	while (l > 0)
	{
		b1 = s[b1];
		l--;
	}
	return b1;
}

#pragma endregion


#pragma region 用数组实现集合的集合运算，n是集合大小


//用数组实现集合的集合运算

int inset(int e, bool* s)
{
	return s[e];
}

//s1并s2
int addset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] || s2[i];
	}
	return 0;
}
//s1-s2
int subset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] && (!s2[i]);
	}
	return 0;
}
//s1交s2
int mulset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] && s2[i];
	}
	return 0;
}
#pragma endregion

#pragma region 出错处理
//出错处理，打印出错位置和错误编码
int error(int n)
{
	//打印的空格
	char space[81];
	memset(space, 32, 81);
	space[cc - 1] = 0; //出错时当前符号已经读完，所以cc-1，0表示'\0'结束

	printf("Error=>%s! %d:%s\n", space, n, err_msg[n]);
	fprintf(fa1, "****%s!%d\n", space, n);

	err++;
	return -1;
}

/*
* 测试当前符号是否合法，短语层恢复思想
*
* 在某一部分（如一条语句，一个表达式）将要结束时时我们希望下一个符号属于某集?
* （该部分的后跟符号），test负责这项检测，并且负责当检测不通过时的补救措施，
* 程序在需要检测时指定当前需要的符号集合和补救用的集合（如之前未完成部分的后跟
* 符号），以及检测不通过时的错误号。
*
* s1:   我们需要的符号
* s2:   如果不是我们需要的，则需要一个补救用的集合
* n:    错误号
*/
int test(bool* s1, bool* s2, int n)
{
	if (!inset(sym, s1))
	{
		error(n);
		/* 当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合 */
		while ((!inset(sym, s1)) && (!inset(sym, s2)))
		{
			getsymdo;
		}
	}
	return 0;
}

#pragma endregion

#pragma region 符号表管理

/*
* 在符号表中加入一项
*
* k:      名字种类const,var or procedure
* ptx:    名字表尾指针的指针，为了可以改变名字表尾指针的值
* lev:    名字所在的层次,，以后所有的lev都是这样
* pdx:    dx为当前应分配的变量的相对地址，分配后要增加1
为什么变量会使指针++,因为变量需要分配空间
*/
void enter(enum object k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name, id); /* 全局变量id中已存有当前名字的名字 */
	table[(*ptx)].kind = k;
	switch (k)
	{
	case constant:  /* 常量名字 */
		if (num > amax)
		{
			error(31);  /* 数越界 */
			num = 0;
		}
		table[(*ptx)].val = num;
		break;
	case variable:  /* 变量名字 */
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;
	case array:  /* 数组名字 */
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		table[(*ptx)].size = num;
		(*pdx) += num;
		break;
	case procedur:  /*　过程名字　*/
		table[(*ptx)].level = lev;
		break;
	}
}

/*
* 查找名字的位置，从后往前，保证先看此过程局部变量再看其他的
* 找到则返回在名字表中的位置,否则返回0.
*
* idt:    要查找的名字
* tx:     当前名字表尾指针
*/
int position(char* idt, int tx)
{
	int i;
	strcpy(table[0].name, idt);
	i = tx;
	while (strcmp(table[i].name, idt) != 0)
	{
		i--;
	}
	return i;
}

/*
* 输出符号表
*/
void ListTable(int tx0, int tx)
{
	int i;
	i = tx;

	while (table[i].kind != procedur && i > 0) i--;

	if (i != tx && i != 0) printf("The Table at procedure %s:\n", table[i].name);
	else  printf("The Table at main program:\n");

	if (tx0 + 1 > tx)
	{
		printf("    NULL\n");
	}
	else {
		printf("%-8s %-8s %-8s %-8s %-8s %-8s \n", "number", "kind", "name", "val/lev", "addr", "size");
		for (i = tx0 + 1; i <= tx; i++)
		{
			switch (table[i].kind)
			{
			case constant:
				printf("%-8d const    %-8s ", i, table[i].name);
				printf("val=%-4d\n", table[i].val);
				fprintf(fas, "%-8d const    %-8s ", i, table[i].name);
				fprintf(fas, "val=%-4d\n", table[i].val);
				break;
			case variable:
				printf("%-8d var      %-8s ", i, table[i].name);
				printf("lev=%-4d addr=%-3d\n", table[i].level, table[i].adr);
				fprintf(fas, "%-8d var      %-8s ", i, table[i].name);
				fprintf(fas, "lev=%-4d addr=%-3d\n", table[i].level, table[i].adr);
				break;
			case procedur:
				printf("%-8d proc     %-8s ", i, table[i].name);
				printf("lev=%-4d addr=%-3d size=%-3d\n", table[i].level, table[i].adr, table[i].size);
				fprintf(fas, "%-8d proc     %-8s ", i, table[i].name);
				fprintf(fas, "lev=%-4d addr=%-3d size=%-3d\n", table[i].level, table[i].adr, table[i].size);
				break;
			case array:
				printf("%-8d array    %-8s ", i, table[i].name);
				printf("lev=%-4d addr=%-3d size=%-3d\n", table[i].level, table[i].adr, table[i].size);
				fprintf(fas, "%-8d array    %-8s ", i, table[i].name);
				fprintf(fas, "lev=%-4d addr=%-3d size=%-3d\n", table[i].level, table[i].adr, table[i].size);
				break;
			}
		}
	}

	printf("\n");
}
#pragma endregion
